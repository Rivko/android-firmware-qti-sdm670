/** vi: tw=128 ts=3 sw=3 et
@file pd_elf.c
@brief This file contains the API details for the Protection Domain Monitor, API 1.0
*/
/*=============================================================================
NOTE: The @brief description above does not appear in the PDF.
The tms_mainpage.dox file contains the group/module descriptions that
are displayed in the output PDF generated using Doxygen and LaTeX. To
edit or update any of the group/module text in the PDF, edit the
tms_mainpage.dox file or contact Tech Pubs.
===============================================================================*/
/*=============================================================================
Copyright (c) 2014 - 2019 by Qualcomm Technologies, Inc.  All Rights Reserved.
Qualcomm Confidential and Proprietary
=============================================================================*/
/*=============================================================================
Edit History
$Header: //components/rel/core.qdsp6/2.1/debugtools/pd_mon/src/pd_elf.c#4 $
$DateTime: 2019/02/18 01:48:20 $
$Change: 18354452 $
$Author: pwbldsvc $
===============================================================================*/

#include "tms_utils_msg.h"
#include "pd_dump.h"
#include "DDITimetick.h"  //DAV
#include "rfs_api.h"

#include "pd_elf.h"
#include "pd_elf_hexagon.h"

extern void dog_force_kick(void);

typedef struct
{
   int stage ;
   void * addr;
   int write_count;
   void * write_addr;
   unsigned int write_size;
   int prg;
   void * prg_addr;

} pd_debug_buf_s;

volatile pd_debug_buf_s pd_debug_buf={0};

// helper prototype
int32 pd_elf_rfs_write_all(int filedes, void *buf, uint32 nbyte);

PD_ELF_STATUS pd_elf_header_init(Elf32_Ehdr* header_p)
{
   PD_ELF_STATUS rc = PD_ELF_STATUS_ERROR;

   if (NULL != header_p)
   {
      secure_memset(header_p, 0, sizeof(*header_p));

      header_p->e_ident[EI_MAG0]    = ELFMAG0;
      header_p->e_ident[EI_MAG1]    = ELFMAG1;
      header_p->e_ident[EI_MAG2]    = ELFMAG2;
      header_p->e_ident[EI_MAG3]    = ELFMAG3;
      header_p->e_ident[EI_CLASS]   = ELFCLASS32;
      header_p->e_ident[EI_DATA]    = ELFDATA2LSB;
      header_p->e_ident[EI_VERSION] = EV_CURRENT;

      header_p->e_ident[EI_OSABI]  = ELFOSABI_NONE;

      header_p->e_type             = ET_CORE;
      header_p->e_machine          = EM_HEXAGON;
      header_p->e_version          = EV_CURRENT;
      header_p->e_ehsize           = sizeof(Elf32_Ehdr);

      header_p->e_shentsize        = sizeof(Elf32_Shdr);
      header_p->e_phentsize        = sizeof(Elf32_Phdr);

      header_p->e_shstrndx         = SHN_UNDEF;

      rc = PD_ELF_STATUS_SUCCESS;
   }

   return rc;
}

static PD_ELF_STATUS pd_elf_header_write(int fd, Elf32_Ehdr* header_p)
{
   PD_ELF_STATUS rc = PD_ELF_STATUS_ERROR;

   if (header_p->e_ehsize == pd_elf_rfs_write_all(fd, (void*)header_p, header_p->e_ehsize))
   {
      rc = PD_ELF_STATUS_SUCCESS;
   }

   return rc;
}

static PD_ELF_STATUS pd_elf_header_free(Elf32_Ehdr* header_p)
{
   PD_ELF_STATUS rc = PD_ELF_STATUS_ERROR;

   pd_debug_buf.prg = 301;

   if (NULL != header_p)
   {
     pd_debug_buf.prg = 302;
           
     secure_memset(header_p, 0, sizeof(*header_p));

      rc = PD_ELF_STATUS_SUCCESS;
   }

   pd_debug_buf.prg = 303;

   return rc;
}

static inline Elf32_Half pd_elf_prgtab_count(Elf32_Ehdr* header_p, pd_elf_prgtab_p* prgtab_p)
{
   pd_elf_prgtab_p pt_p;
   Elf32_Half count = 0;

   for (pt_p = *prgtab_p; NULL != pt_p; pt_p = pt_p->next)
   {
      count++;
   }

   return count;
}

static inline Elf32_Off pd_elf_prgtab_size(Elf32_Ehdr* header_p, pd_elf_prgtab_p* prgtab_p)
{
   pd_elf_prgtab_p pt_p;
   Elf32_Off size = 0;

   for (pt_p = *prgtab_p; NULL != pt_p; pt_p = pt_p->next)
   {
      size += pt_p->phdr->p_filesz;
   }

   return size;
}

PD_ELF_STATUS pd_elf_prgtab_alloc(Elf32_Ehdr* header_p, pd_elf_prgtab_p* prgtab_p, Elf32_Word type, char* name, Elf32_Word ntype, void* addr, Elf32_Word size)
{
   PD_ELF_STATUS rc = PD_ELF_STATUS_ERROR;
   pd_elf_prgtab_p pt_p = malloc(sizeof(pd_elf_prgtab_t));

   TMS_MSG_ERROR_2("prgtab_alloc 0x%x 0x%x", addr, size);

   if (NULL != pt_p)
   {
      secure_memset(pt_p, 0, sizeof(pd_elf_prgtab_t));

      //pt_p->phdr.p_offset = 0;
      pt_p->p_type = type;
      //pt_p->phdr.p_type = type;
      //pt_p->phdr.p_align = 0;
      pt_p->addr_type = PD_ELF_BUFFER;

      switch (type)
      {
         case PT_LOAD:
         {
            pt_p->size = size;
            pt_p->addr = addr;
            
            //pt_p->phdr.p_vaddr = (Elf32_Addr)addr;
            //pt_p->phdr.p_filesz = size;
            //pt_p->phdr.p_memsz = size;
            //pt_p->phdr.p_flags = PF_R + PF_W;

            pt_p->next = *prgtab_p;

            *prgtab_p = pt_p;

            rc = PD_ELF_STATUS_SUCCESS;

            break;
         }

         case PT_NOTE:
         {
            Elf32_Word name_len = strlen(name) + 1;
            Elf32_Word name_pad = 0;
            Elf32_Word desc_pad = 0;

            if (0 != name_len % sizeof(Elf32_Word))
            {
               name_pad = sizeof(Elf32_Word) - name_len % sizeof(Elf32_Word);
            }

            pt_p->size = sizeof(Elf32_Nhdr) + name_len + name_pad;
            /* Calculate padding for desc if required */
            if (0 != size % sizeof(Elf32_Word))
            {
               desc_pad = sizeof(Elf32_Word) - size % sizeof(Elf32_Word);
            }
            pt_p->size += size + desc_pad;

            pt_p->addr = malloc(pt_p->size);

            if (NULL != pt_p->addr)
            {
               secure_memset(pt_p->addr, 0, pt_p->size);

               ((Elf32_Nhdr*)pt_p->addr)->n_namesz = name_len;
               ((Elf32_Nhdr*)pt_p->addr)->n_descsz = size;
               ((Elf32_Nhdr*)pt_p->addr)->n_type = ntype;

               memsmove((char*)pt_p->addr + sizeof(Elf32_Nhdr), name_len, name, name_len);

               memsmove((char*)pt_p->addr + sizeof(Elf32_Nhdr) + name_len + name_pad, size, addr, size);
       
               if (desc_pad !=0)
                  memset((char*)pt_p->addr + sizeof(Elf32_Nhdr) + name_len + name_pad + size, 0, desc_pad);

               //pt_p->phdr.p_vaddr = 0;
               //pt_p->phdr.p_filesz = pt_p->size;
               //pt_p->phdr.p_memsz = 0;
               //pt_p->phdr.p_flags = 0;

               pt_p->next = *prgtab_p;

               *prgtab_p = pt_p;

               rc = PD_ELF_STATUS_SUCCESS;

               break;
            }

            else
            {
               free(pt_p);
            }

            break;
         }
      }
   }

   return rc;
}

//takes a linked list of memory blocks and maps them to a specified virtual address
PD_ELF_STATUS pd_elf_prgtab_alloc_list(Elf32_Ehdr* header_p, pd_elf_prgtab_p* prgtab_p, uint32 list_sz, void* head_ptr, uint32 data_sz, void* virt_addr, uint32 virt_sz)
{
   PD_ELF_STATUS rc = PD_ELF_STATUS_ERROR;
   pd_elf_prgtab_p pt_p;

   //ensure adequate virtual space
   if (virt_sz < list_sz)
   {
      TMS_MSG_ERROR_2("Insufficient vmem 0x%x/0x%x", list_sz, virt_sz);
   }
   else
   {
      pt_p = malloc(sizeof(pd_elf_prgtab_t));

      if (NULL != pt_p)
      {
         secure_memset(pt_p, 0, sizeof(pd_elf_prgtab_t));

         pt_p->addr_type = PD_ELF_LIST;

         //pt_p->phdr.p_offset = 0;
         //pt_p->phdr.p_type = PT_LOAD;
         //pt_p->phdr.p_align = 0;

         pt_p->size = list_sz;
         pt_p->addr = head_ptr;
         //pt_p->list_size = list_sz;

         pt_p->unmapped_addr = (Elf32_Addr)virt_addr;
         //pt_p->phdr.p_vaddr = (Elf32_Addr)virt_addr;
         //pt_p->phdr.p_filesz = size;
         //pt_p->phdr.p_memsz = pt_p->phdr.p_filesz;
         //pt_p->phdr.p_flags = PF_R + PF_W;

         pt_p->next = *prgtab_p;

         *prgtab_p = pt_p;

         rc = PD_ELF_STATUS_SUCCESS;
      }
      else
      {
         TMS_MSG_ERROR_2("Malloc failure 0x%x 0x%x", virt_addr, list_sz);
      }
   }

   return rc;
}

PD_ELF_STATUS pd_elf_prgtab_prepare_header(Elf32_Ehdr* header_p, pd_elf_prgtab_p* prgtab_p, void ** prgtab_phdr, unsigned int prgtab_phdr_num )
{
   Elf32_Phdr * phdr_t;
   pd_elf_prgtab_p pt_p;
   
   /* Check for overflow */
   if ( prgtab_phdr_num > UINT32_MAX / sizeof(Elf32_Phdr) )
     return PD_ELF_STATUS_ERROR;
   
   /* Allocate prghdr */
   *prgtab_phdr = malloc( prgtab_phdr_num * sizeof(Elf32_Phdr) );
  
   if ( *prgtab_phdr == NULL )
     return PD_ELF_STATUS_ERROR;

   secure_memset(*prgtab_phdr, 0, prgtab_phdr_num * sizeof(Elf32_Phdr));

   phdr_t = *prgtab_phdr;
   pt_p = *prgtab_p;

   /* Walk prghdr and populate it */
   while ( pt_p != NULL )
   {
      if ( pt_p->addr_type == PD_ELF_LIST )
      {
         phdr_t->p_offset = 0;
         phdr_t->p_type = PT_LOAD;
         phdr_t->p_align = 0;

         phdr_t->p_vaddr = pt_p->unmapped_addr;
         phdr_t->p_filesz = pt_p->size;
         phdr_t->p_memsz = pt_p->size;
         phdr_t->p_flags = PF_R + PF_W;

      }
      else
      {
         phdr_t->p_offset = 0;
         phdr_t->p_type = pt_p->p_type;
         phdr_t->p_align = 0;
      
         if (pt_p->p_type == PT_LOAD )
         {
           phdr_t->p_vaddr = (Elf32_Addr)pt_p->addr;
           phdr_t->p_filesz = pt_p->size;
           phdr_t->p_memsz = pt_p->size;
           phdr_t->p_flags = PF_R + PF_W;

         }
         else
         {
           phdr_t->p_vaddr = 0;
           phdr_t->p_filesz = pt_p->size;
           phdr_t->p_memsz = 0;
           phdr_t->p_flags = 0;
         }
      }      

      /* Assign Physical address to be same as virtual address */
      phdr_t->p_paddr = phdr_t->p_vaddr;

      pt_p->phdr = phdr_t;
      phdr_t++;
      pt_p = pt_p->next;
   }

   return PD_ELF_STATUS_SUCCESS;
   
}

static PD_ELF_STATUS pd_elf_prgtab_prepare_offsets(Elf32_Ehdr* header_p, pd_elf_prgtab_p* prgtab_p, pd_elf_sectab_p* sectab_p)
{
   PD_ELF_STATUS rc = PD_ELF_STATUS_ERROR;
   pd_elf_prgtab_p pt_p;
   Elf32_Off offset;

   header_p->e_phnum = pd_elf_prgtab_count(header_p, prgtab_p);
   header_p->e_phoff = header_p->e_ehsize;

   offset = header_p->e_phoff + header_p->e_phentsize * header_p->e_phnum;

   for (pt_p = *prgtab_p; NULL != pt_p; pt_p = pt_p->next)
   {
      if (0 != pt_p->phdr->p_filesz)
      {
         pt_p->phdr->p_offset = offset;

         offset += pt_p->phdr->p_filesz;
      }
   }

   rc = PD_ELF_STATUS_SUCCESS;

   return rc;
}

PD_ELF_STATUS pd_elf_prgtab_write(int fd, Elf32_Ehdr* header_p, pd_elf_prgtab_p* prgtab_p, void ** prgtab_phdr)
{
   PD_ELF_STATUS rc = PD_ELF_STATUS_ERROR;
   pd_elf_prgtab_p pt_p;
   void **link;
   unsigned int size;

   // Write ALL Headers
   
   pd_debug_buf.prg = 1;

   if (sizeof(Elf32_Phdr) * header_p->e_phnum == pd_elf_rfs_write_all(fd, *prgtab_phdr, sizeof(Elf32_Phdr) * header_p->e_phnum))
   {
     // SUCCESS
   }
   else
   {
     return rc; // for ()
   }

   // Write ALL Sections
   pd_debug_buf.prg = 2;

   for (pt_p = *prgtab_p; NULL != pt_p; pt_p = pt_p->next)
   {
      pd_debug_buf.prg = 3;
      if (PD_ELF_LIST == pt_p->addr_type)
      {
         // walk the linked list dumping block by block
         for (link = pt_p->addr; NULL != link; link = (void**)(*link))
         {
            pd_debug_buf.prg = 4;
            pd_debug_buf.prg_addr = pt_p->addr;
            size = *(unsigned int*)((char *)link + sizeof(void*));
            if (size == pd_elf_rfs_write_all(fd, (char*)link + sizeof(void*) +sizeof(unsigned int), size))
            {
               // SUCCESS
            }
            else
            {
               break; // for (link)
            }
         }
         if (NULL != link) //early exit?
         {
            break; // for (pt_p)
         }
      }
      else
      {
         pd_debug_buf.prg = 5;
         //pd_debug_buf.prg_addr = pt_p->addr;
         if (0 != pt_p->phdr->p_filesz)
         {
            pd_debug_buf.prg = 6;
            pd_debug_buf.prg_addr = pt_p->addr;
            if (pt_p->phdr->p_filesz == pd_elf_rfs_write_all(fd, (void*)pt_p->addr, pt_p->phdr->p_filesz))
            {
               // SUCCESS
            }
            else
            {
               break; // for ()
            }
         }
      }
   }

   pd_debug_buf.prg = 7;

   rc = PD_ELF_STATUS_SUCCESS;

   return rc;
}

PD_ELF_STATUS pd_elf_prgtab_free(Elf32_Ehdr* header_p, pd_elf_prgtab_p* prgtab_p, void ** prgtab_phdr)
{
   PD_ELF_STATUS rc = PD_ELF_STATUS_ERROR;
   void **temp, **link;

   pd_debug_buf.prg = 101;
   while (NULL != *prgtab_p)
   {
      pd_elf_prgtab_p pt_p = *prgtab_p;

      *prgtab_p = pt_p->next;

      pd_debug_buf.prg = 102;

      if ( pt_p->phdr )
      {
        switch (pt_p->phdr->p_type)
        {
           case PT_LOAD:
              pd_debug_buf.prg = 103;
              if (PD_ELF_LIST==pt_p->addr_type)
              {
                 pd_debug_buf.prg = 104;
                 for (link = pt_p->addr; NULL != link; /*inline*/)
                 {
                    temp = link;
                    link = (void**)(*link);
                    pd_debug_buf.prg = 105;
                    pd_debug_buf.prg_addr = temp;
                    free(temp);
                 }
              }
              else
              {
                 /* nothing malloc'd */
              }
              break;

           case PT_NOTE:
              pd_debug_buf.prg = 106;
              pd_debug_buf.prg_addr = pt_p->addr;
              free(pt_p->addr);
              break;
        }
      }
      pd_debug_buf.prg = 107;
      pd_debug_buf.prg_addr = pt_p;
      free(pt_p);
   }

   if(*prgtab_phdr != NULL)
     free(*prgtab_phdr);
   pd_debug_buf.prg = 108;
   rc = PD_ELF_STATUS_SUCCESS;

   return rc;
}

static inline Elf32_Half pd_elf_sectab_count(Elf32_Ehdr* header_p, pd_elf_sectab_p *sectab_p)
{
   pd_elf_sectab_p st_p;
   Elf32_Half count = 0;

   for (st_p = *sectab_p; NULL != st_p; st_p = st_p->next)
   {
      count++;
   }

   return count;
}

//static inline Elf32_Off pd_elf_sectab_size(pd_elf_sectab_p* sectab_p)
//{
//   pd_elf_sectab_p st_p;
//   Elf32_Off size = 0;
//
//   for (st_p = *sectab_p; NULL != st_p; st_p = st_p->next)
//   {
//      size += st_p->shdr.sh_size;
//   }
//
//   return size;
//}

PD_ELF_STATUS pd_elf_sectab_alloc(Elf32_Ehdr* header_p, pd_elf_sectab_p* sectab_p, Elf32_Word type, Elf32_Word link)
{
   PD_ELF_STATUS rc = PD_ELF_STATUS_ERROR;
   pd_elf_sectab_p st_p = malloc(sizeof(pd_elf_sectab_t));

   if (NULL != st_p)
   {
      secure_memset(st_p, 0, sizeof(pd_elf_sectab_t));

      st_p->shdr.sh_offset = 0;
      st_p->shdr.sh_type = type;
      st_p->shdr.sh_link = link;

      st_p->next = *sectab_p;

      *sectab_p = st_p;

      rc = PD_ELF_STATUS_SUCCESS;
   }

   return rc;
}

static PD_ELF_STATUS pd_elf_sectab_prepare_offsets(Elf32_Ehdr* header_p, pd_elf_prgtab_p* prgtab_p, pd_elf_sectab_p* sectab_p)
{
   PD_ELF_STATUS rc = PD_ELF_STATUS_ERROR;
   pd_elf_sectab_p st_p;
   Elf32_Off offset;

   header_p->e_shnum = pd_elf_sectab_count(header_p, sectab_p);
   header_p->e_shoff = header_p->e_phoff + header_p->e_phentsize * header_p->e_phnum + pd_elf_prgtab_size(header_p, prgtab_p);

   offset = header_p->e_shoff + header_p->e_shentsize * header_p->e_shnum;

   for (st_p = *sectab_p; NULL != st_p; st_p = st_p->next)
   {
      if (0 != st_p->shdr.sh_size)
      {
         st_p->shdr.sh_offset = offset;

         offset += st_p->shdr.sh_size;
      }
   }

   rc = PD_ELF_STATUS_SUCCESS;

   return rc;
}

PD_ELF_STATUS pd_elf_sectab_write(int fd, Elf32_Ehdr* header_p, pd_elf_sectab_p* sectab_p)
{
   PD_ELF_STATUS rc = PD_ELF_STATUS_ERROR;
   pd_elf_sectab_p st_p;

   for (st_p = *sectab_p; NULL != st_p; st_p = st_p->next)
   {
      if (sizeof(Elf32_Shdr) == pd_elf_rfs_write_all(fd, (void*)&st_p->shdr, sizeof(Elf32_Shdr)))
      {
         // SUCCESS
      }
      else
      {
         break; // for ()
      }
   }

   if (NULL == st_p)
   {
      for (st_p = *sectab_p; NULL != st_p; st_p = st_p->next)
      {
         if (0 != st_p->shdr.sh_size)
         {
            if (st_p->shdr.sh_size == pd_elf_rfs_write_all(fd, (void*)st_p->shdr.sh_addr, st_p->shdr.sh_size))
            {
               // SUCCESS
            }
            else
            {
               break; // for ()
            }
         }
      }

      if (NULL == st_p)
      {
         rc = PD_ELF_STATUS_SUCCESS;
      }
   }

   return rc;
}

static PD_ELF_STATUS pd_elf_sectab_free(Elf32_Ehdr* header_p, pd_elf_sectab_p* sectab_p)
{
   PD_ELF_STATUS rc = PD_ELF_STATUS_ERROR;

   pd_debug_buf.prg = 201;
           
   while (NULL != *sectab_p)
   {
      pd_elf_sectab_p st_p = *sectab_p;

      *sectab_p = st_p->next;

      pd_debug_buf.prg = 202;
      pd_debug_buf.prg_addr = st_p;

      free(st_p);
   }

   pd_debug_buf.prg = 203;
   rc = PD_ELF_STATUS_SUCCESS;

   return rc;
}

PD_ELF_STATUS pd_elf_init(Elf32_Ehdr* header_p, pd_elf_prgtab_p* prgtab_p, pd_elf_sectab_p* sectab_p)
{
   PD_ELF_STATUS rc = PD_ELF_STATUS_ERROR;

   if (PD_ELF_STATUS_SUCCESS == pd_elf_header_init(header_p))
   {
      rc = PD_ELF_STATUS_SUCCESS;
   }

   return rc;
}

PD_ELF_STATUS pd_elf_prepare_offsets(Elf32_Ehdr* header_p, pd_elf_prgtab_p* prgtab_p, pd_elf_sectab_p* sectab_p)
{
   PD_ELF_STATUS rc = PD_ELF_STATUS_ERROR;

   if (PD_ELF_STATUS_SUCCESS == pd_elf_prgtab_prepare_offsets(header_p, prgtab_p, sectab_p) &&
       PD_ELF_STATUS_SUCCESS == pd_elf_sectab_prepare_offsets(header_p, prgtab_p, sectab_p))
   {
      rc = PD_ELF_STATUS_SUCCESS;
   }

   return rc;
}

PD_ELF_STATUS pd_elf_write(int fd, Elf32_Ehdr* header_p, pd_elf_prgtab_p* prgtab_p, pd_elf_sectab_p* sectab_p, void ** prgtab_phdr)
{
   PD_ELF_STATUS rc = PD_ELF_STATUS_ERROR;
   // DalDeviceHandle *hTimerHandle = NULL;
   // uint64 start, end;

   // if (DalTimetick_Attach("SystemTimer", &hTimerHandle)) {;}
   // if (DalTimetick_GetTimetick64(hTimerHandle, &start)) {;}
   pd_debug_buf.stage = 1;
   pd_debug_buf.addr  = (void*)header_p;
   
   if (PD_ELF_STATUS_SUCCESS != pd_elf_header_write(fd, header_p))
   {
      pd_debug_buf.stage = 2;
      
      TMS_MSG_ERROR( "Elf write header error");
      return rc;
   }

   pd_debug_buf.stage = 3;
    pd_debug_buf.addr  = (void*)prgtab_p;
  
   if (PD_ELF_STATUS_SUCCESS != pd_elf_prgtab_write(fd, header_p, prgtab_p, prgtab_phdr))
   {
      pd_debug_buf.stage = 4;
      TMS_MSG_ERROR( "Elf write prgtab error");
      return rc;
   }

   pd_debug_buf.stage = 5;
   pd_debug_buf.addr  = (void*)sectab_p;
   
   if (PD_ELF_STATUS_SUCCESS != pd_elf_sectab_write(fd, header_p, sectab_p))
   {
      pd_debug_buf.stage = 6;
      TMS_MSG_ERROR( "Elf write sectab error");
      return rc;
   }
   pd_debug_buf.stage = 7;
   
   // DalTimetick_GetTimetick64(hTimerHandle, &end);

   rc = PD_ELF_STATUS_SUCCESS;
   return rc;
}

PD_ELF_STATUS pd_elf_term(Elf32_Ehdr* header_p, pd_elf_prgtab_p* prgtab_p, pd_elf_sectab_p* sectab_p, void **prgtab_phdr)
{
   PD_ELF_STATUS rc = PD_ELF_STATUS_ERROR;

   if (PD_ELF_STATUS_SUCCESS == pd_elf_prgtab_free(header_p, prgtab_p, prgtab_phdr) &&
       PD_ELF_STATUS_SUCCESS == pd_elf_sectab_free(header_p, sectab_p) &&
       PD_ELF_STATUS_SUCCESS == pd_elf_header_free(header_p))
   {
      rc = PD_ELF_STATUS_SUCCESS;
   }

   return rc;
}

// write-to-completion helper for "int32 rfs_write (int filedes, const void *buf, uint32 nbyte);"
int32 pd_elf_rfs_write_all(int filedes, void *buf, uint32 nbyte)
{
   int32 written, pending, fails;
   const byte * out_p;

   fails = 0;
   out_p = buf;
   pending = nbyte;

   pd_debug_buf.write_count++;
   pd_debug_buf.write_addr = buf;
   pd_debug_buf.write_size = nbyte;

   while (pending != 0)
   {
      dog_force_kick();
      written = rfs_write(filedes, out_p, pending);

      // check for error 
      if (written <= 0)
      {
         return written;
      }

      out_p += written;
      pending -= written;
   }

   return nbyte; 
}
