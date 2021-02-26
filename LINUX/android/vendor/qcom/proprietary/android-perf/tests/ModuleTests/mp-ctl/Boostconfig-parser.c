/******************************************************************************
  @file    Boostconfig-parser.cpp
  @brief   Implementation for parsing perfboostsconfig.xml to support
           perfHint Verification

  DESCRIPTION

  ---------------------------------------------------------------------------
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/
#include <stdio.h>
#include <string.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

#ifdef LIBXML_TREE_ENABLED

typedef enum {
ID = 0,
TYPE,
ENABLED,
TARGET,
RESOLUTION,
RESOURCES,
INVALID_TAG = -1,
}tags;

int hash_val(const char* inString);

/*
 * Ensure libxml2 is installed in your machine.
 * Else install using  "sudo apt-get install libxml2-dev"
 * To compile this file using gcc you can type
 * gcc -o boostconfig-parser `xml2-config --cflags` Boostconfig-parser.c `xml2-config --libs`
 */

static void parse_boost_configxml(xmlNode* root_node, FILE* fp)
{
   xmlNode* cur_node = NULL;
   xmlChar* id_attr_value = NULL;
   xmlChar* type_attr_value = NULL;
   xmlChar* enabled_attr_value = NULL;
   xmlChar* target_attr_value = NULL;
   xmlChar* resolution_attr_value = NULL;
   xmlChar* resources_attr_value = NULL;

   for (cur_node = root_node; cur_node; cur_node = cur_node->next)
   {
       if (cur_node->type == XML_ELEMENT_NODE)
       {
           xmlAttr* attribute = cur_node->properties;
           while(attribute)
           {

               switch(hash_val(attribute->name))
               {
                   case ID:
                       id_attr_value = xmlGetProp(cur_node, attribute->name);
                       break;
                   case TYPE:
                       type_attr_value = xmlGetProp(cur_node, attribute->name);
                       break;
                   case ENABLED:
                       enabled_attr_value = xmlGetProp(cur_node, attribute->name);
                       break;
                   case TARGET:
                       target_attr_value = xmlGetProp(cur_node, attribute->name);
                       break;
                   case RESOLUTION:
                       resolution_attr_value = xmlGetProp(cur_node, attribute->name);
                       break;
                   case RESOURCES:
                       resources_attr_value = xmlGetProp(cur_node, attribute->name);

                       if(enabled_attr_value && !strncmp(enabled_attr_value,"true",strlen("true")))
                       {
                           if (id_attr_value && type_attr_value && target_attr_value && resources_attr_value && resolution_attr_value)
                           {
                               if(!xmlHasProp(cur_node, BAD_CAST"Resolution"))
                                   fprintf(fp,"Id_%s;Type_%s;Target_%s;Resolution_None;%s\n",id_attr_value,type_attr_value,target_attr_value,resources_attr_value);
                               else
                                   fprintf(fp,"Id_%s;Type_%s;Target_%s;Resolution_%s;%s\n",id_attr_value,type_attr_value,target_attr_value,resolution_attr_value,resources_attr_value);
                           }
                       }
                       break;
               }
               attribute = attribute->next;
           }
       }
       parse_boost_configxml(cur_node->children, fp);
   }

   xmlFree(id_attr_value);
   xmlFree(type_attr_value);
   xmlFree(enabled_attr_value);
   xmlFree(target_attr_value);
   xmlFree(resolution_attr_value);
   xmlFree(resources_attr_value);
}

int hash_val(const char* inString)
{
    if(!strncmp(inString, "Id", strlen("Id")))
       return ID;
    else if(!strncmp(inString, "Type", strlen("Type")))
       return TYPE;
    else if(!strncmp(inString, "Enable", strlen("Enable")))
       return ENABLED;
    else if(!strncmp(inString, "Target", strlen("Target")))
       return TARGET;
    else if(!strncmp(inString, "Resolution", strlen("Resolution")))
       return RESOLUTION;
    else if(!strncmp(inString, "Resources", strlen("Resources")))
       return RESOURCES;
    else
       return INVALID_TAG;
}

int main(int argc, char **argv)
{
    xmlDoc *doc = NULL;
    xmlNode *root_element = NULL;
    FILE *fp = NULL;

    /*
     * Initialize the library and check potential ABI mismatches
     * between the version it was compiled for and the actual shared
     * library used.
     */
    LIBXML_TEST_VERSION

    /*parse the file and get the DOM */
    doc = xmlReadFile("/vendor/etc/perf/perfboostsconfig.xml", NULL, 0);

    if (doc == NULL)
    {
        printf("Error: could not parse file \n");
        return 0;
    }
    else
    {
        fp = fopen("/data/nativetest/boostConfigParser/perfHintConfig.txt" , "w");

        if (!fp)
            return 0;

        /*Get the root element node */
        root_element = xmlDocGetRootElement(doc);

        /* Parse perfboostsconfig.xml file */
        parse_boost_configxml(root_element, fp);

        fprintf(fp, "%s","END");
        fclose(fp);

        /*free the document */
        xmlFreeDoc(doc);

        /*
         *Free the global variables that may
         *have been allocated by the parser.
         */
        xmlCleanupParser();

        return 1;
    }
}
#else
int main(void)
{
    printf("LIBXML_TREE_ENABLED Feature is Not present.... Boostconfig-parser not compiled.... \n");
    exit(1);
}
#endif
