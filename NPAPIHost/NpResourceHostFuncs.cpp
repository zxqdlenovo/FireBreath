/**********************************************************\ 
Original Author: Richard Bateman (taxilian)

Created:    Oct 15, 2009
License:    Eclipse Public License - Version 1.0
            http://www.eclipse.org/legal/epl-v10.html

Copyright 2009 Richard Bateman, Firebreath development team
\**********************************************************/


#include "npupp.h"
#include "npruntime.h"

#include <map>
#include <vector>
#include <string>

#include "TypeIDMap.h"

#include "NpapiHost.h"

using namespace FB::Npapi;

/* NPN_MemAlloc */
void* NP_LOADDS NpapiHost::NH_MemAlloc(uint32 size)
{
    return malloc(size);
}

/* NPN__MemFree */
void NP_LOADDS NpapiHost::NH_MemFree(void* ptr)
{
    free(ptr);
}

/* NPN_MemFlush */
uint32 NP_LOADDS NpapiHost::NH_MemFlush(uint32 size)
{
    return 0;
}

/* NPN_CreateObject */

NPObject* NP_LOADDS NpapiHost::NH_CreateObject(NPP npp, NPClass *aClass)
{
    NPObject *newObj(NULL);
    if (aClass->allocate) {
        newObj = aClass->allocate(npp, aClass);
    } else {
        newObj = (NPObject *)malloc(sizeof(NPObject));
    }
    newObj->referenceCount = 1;
    newObj->_class = aClass;
    return newObj;
}

/* NPN_RetainObject */
NPObject* NP_LOADDS NpapiHost::NH_RetainObject(NPObject *obj)
{
    obj->referenceCount++;
    return obj;
}

/* NPN_ReleaseObject */
void NP_LOADDS NpapiHost::NH_ReleaseObject(NPObject *obj)
{
    if (--(obj->referenceCount) == 0) {
        if (obj->_class->deallocate) {
            obj->_class->deallocate(obj);
        } else {
            free(obj);
        }
    }
}

/* NPN_ReleaseVariantValue */
void NP_LOADDS NpapiHost::NH_ReleaseVariantValue(NPVariant *variant)
{
    if (variant->type == NPVariantType_String) {
		NH_MemFree((void*)variant->value.stringValue.utf8characters);
    } else if (variant->type == NPVariantType_Object) {
        NH_ReleaseObject(variant->value.objectValue);
    }
}



/* NPN_GetStringIdentifier */
NPIdentifier NP_LOADDS NpapiHost::NH_GetStringIdentifier(const NPUTF8* name)
{
    std::string sName(name);
    return m_idMapper.getIdForValue(sName);
}

/* NPN_GetStringIdentifiers */
void NP_LOADDS NpapiHost::NH_GetStringIdentifiers(const NPUTF8** names, int32_t nameCount, NPIdentifier* identifiers)
{
    for (int32_t i = 0; i < nameCount; i++) {
        identifiers[i] = NH_GetStringIdentifier(names[i]);
    }
}

/* NPN_GetIntIdentifier */
NPIdentifier NP_LOADDS NpapiHost::NH_GetIntIdentifier(int32_t intid)
{
    return m_idMapper.getIdForValue(intid);;
}

/* NPN_IdentifierIsString */
bool NP_LOADDS NpapiHost::NH_IdentifierIsString(NPIdentifier identifier)
{
    return m_idMapper.idIsType<std::string>(identifier);
}

/* NPN_UTF8FromIdentifier */
NPUTF8* NP_LOADDS NpapiHost::NH_UTF8FromIdentifier(NPIdentifier identifier)
{
    try {
        std::string str = m_idMapper.getValueForId<std::string>(identifier);
        NPUTF8 *outStr = (NPUTF8*)NH_MemAlloc(str.size() + 1);
        memcpy(outStr, str.c_str(), str.size() + 1);
        return outStr;
    } catch (...) {
        return NULL;
    }
}

/* NPN_IntFromIdentifier */
int32_t NP_LOADDS NpapiHost::NH_IntFromIdentifier(NPIdentifier identifier)
{
    try {
        return m_idMapper.getValueForId<int32_t>(identifier);
    } catch (...) {
        return 0;
    }
}