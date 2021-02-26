LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

# ---------------------------------------------------------------------------------
#                 Exporting MM-Parser include files to public folder(s)
# ---------------------------------------------------------------------------------
LOCAL_COPY_HEADERS_TO := mm-parser/include
LOCAL_COPY_HEADERS += inc/parserdatadef.h
LOCAL_COPY_HEADERS += inc/DataSourcePort.h
LOCAL_COPY_HEADERS += inc/SourceBase.h
LOCAL_COPY_HEADERS += inc/filesourcetypes.h
LOCAL_COPY_HEADERS += inc/filesource.h
LOCAL_COPY_HEADERS += inc/sidxparserdatatypes.h
LOCAL_COPY_HEADERS += inc/sidxparser.h

include $(BUILD_COPY_HEADERS)
