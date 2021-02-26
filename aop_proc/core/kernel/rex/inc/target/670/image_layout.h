#ifndef IMAGE_LAYOUT_H
#define IMAGE_LAYOUT_H

#define AOP_STACK_FILL_WORD    0xDDDDDDDD

// This header is the API for components who have sections of data
// in special locations to find out where that data is in this particular image.
// Sections go from section_base to the byte before section_end. It is the clients'
// code's responsibility to validate that the space allocated to them for their section
// is big enough.

#if 0
typedef struct
{
    char* section_base;
    char* section_end;
} rpm_image_section_info;

typedef enum
{
    RPM_DATA_RAM,
    RPM_IMAGE_INFO_HEADER,
    RPM_RAIL_RESIDENCIES,
    RPM_SLEEP_STATS,
    RPM_CPR_STATS,
    RPM_STACK,
    RPM_HEAP,
    RPM_LOG,
    RPM_IMAGE_SECTIONS_COUNT
} rpm_image_section_name;

extern const rpm_image_section_info rpm_image_section_array[RPM_IMAGE_SECTIONS_COUNT];

typedef enum
{
    RPM_HEAP_SECTION_PRIORITY_HIGH,
    RPM_HEAP_SECTION_PRIORITY_MID,
    RPM_HEAP_SECTION_PRIORITY_LOW,
    RPM_HEAP_SECTION_PRIORITY_COUNT,
} rpm_heap_section_priority;

void rpm_heap_add_section(char* base, char* limit, rpm_heap_section_priority priority);

void rpm_adjust_free_heap_space(int heap_adjust);

int get_free_heap_space(void);

void rpm_free_heap_space_init(void);
void rpm_reclaim_data_ram_pool(void);
void rpm_reclaim_code_ram_pool(void);
void rpm_update_stack_usage(void);
#endif

void aop_heap_add_section(char* base, char* limit);

#endif //IMAGE_LAYOUT_H
