/**********************************************************\ 
Original Author: Richard Bateman (taxilian)

Created:    Oct 29, 2009
License:    Eclipse Public License - Version 1.0
            http://www.eclipse.org/legal/epl-v10.html

Copyright 2009 Richard Bateman, Firebreath development team
\**********************************************************/

#include "JSAPISimple.h"

using namespace FB;
JSAPISimple::JSAPISimple(void)
{
    registerMethod( "toString", (CallMethodPtr)&JSAPISimple::callToString );
    registerMethod( "fireEvent", (CallMethodPtr)&JSAPISimple::callFireEvent );
    
    registerProperty( "valid", (GetPropPtr)&JSAPISimple::getValid, NULL );
}

JSAPISimple::~JSAPISimple(void)
{
}


variant JSAPISimple::callFireEvent(std::vector<FB::variant>& args)
{
    try {
        std::string event = args[0].convert_cast<std::string>();
        args.erase(args.begin());
        this->FireEvent(event, args);
        return event;
    } catch (...) {
        throw invalid_arguments();
    }
}

// Example function call and read-only property; override these if desired in derived classes
variant JSAPISimple::callToString(std::vector<FB::variant>& args)
{
    return "JSAPI Javascript Object";
}

variant JSAPISimple::getValid()
{
    return m_valid;
}


// Methods for registering properties and functions to the auto-table
void JSAPISimple::registerMethod(std::string name, CallMethodPtr func)
{
    m_methodMap[name].callFunc = func;
}

void JSAPISimple::registerProperty(std::string name, GetPropPtr getFunc, SetPropPtr setFunc)
{
    m_propertyMap[name].getFunc = getFunc;
    m_propertyMap[name].setFunc = setFunc;
}

// Methods to query existance of members on the API
bool JSAPISimple::HasMethod(std::string methodName)
{
    if (!m_valid)
        return false;

    MethodMap::iterator fnd = m_methodMap.find(methodName);
    if (fnd != m_methodMap.end()) {
        return true;
    } else {
        return false;
    }
}

bool JSAPISimple::HasProperty(std::string propertyName)
{
    if (!m_valid)
        return false;

    PropertyMap::iterator fnd = m_propertyMap.find(propertyName);
    if (fnd != m_propertyMap.end()) {
        return true;
    } else {
        return false;
    }
    return false;
}

bool JSAPISimple::HasEvent(std::string eventName)
{
    EventSingleMap::iterator fnd = m_defEventMap.find(eventName);
    if (fnd != m_defEventMap.end()) {
        return true;
    } else {
        return false;
    }
}


// Methods to manage properties on the API
variant JSAPISimple::GetProperty(std::string propertyName)
{
    if (!m_valid)
        throw object_invalidated();

    PropertyMap::iterator fnd = m_propertyMap.find(propertyName);
    if (fnd != m_propertyMap.end() && fnd->second.getFunc != NULL) {
        return (this->*fnd->second.getFunc)();
    } else {
        throw invalid_member(propertyName);
    }
}

void JSAPISimple::SetProperty(std::string propertyName, variant value)
{
    if (!m_valid)
        throw object_invalidated();

    PropertyMap::iterator fnd = m_propertyMap.find(propertyName);
    if (fnd->second.setFunc != NULL) {
        (this->*fnd->second.setFunc)(value);
    } else {
        throw invalid_member(propertyName);
    }
}

bool JSAPISimple::HasProperty(int idx)
{
    if (!m_valid)
        throw object_invalidated();

    // By default do not support indexing
    // To use array style access, override this method in your API object
    return false;
}

variant JSAPISimple::GetProperty(int idx)
{
    if (!m_valid)
        throw object_invalidated();

    // By default do not support indexing
    // To use array style access, override this method in your API object
    throw invalid_member("Array index: " + variant(idx).convert_cast<std::string>());
}

void JSAPISimple::SetProperty(int idx, variant value)
{
    if (!m_valid)
        throw object_invalidated();

    // By default do not support indexing
    // To use array style access, override this method in your API object
    throw invalid_member("Array index: " + variant(idx).convert_cast<std::string>());
}


// Methods to manage methods on the API
variant JSAPISimple::Invoke(std::string methodName, std::vector<FB::variant>& args)
{
    if (!m_valid)
        throw object_invalidated();

    MethodMap::iterator fnd = m_methodMap.find(methodName);
    if (fnd != m_methodMap.end() && fnd->second.callFunc != NULL) {
        return (this->*fnd->second.callFunc)(args);
    } else {
        throw invalid_member(methodName);
    }    
}