/**
 * Copyright 2013 Thinkbox Software Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * This file contains a Houdini 12 plugin for exporting Houdini particles in PRT format.
 */
 
#define OPENEXR_DLL
#include <deque>

#ifdef WIN32
#define NOMINMAX
#include <windows.h>
#endif

#include <PY/PY_CPythonAPI.h>
#include <UT/UT_DSOVersion.h>
#include <PY/PY_Python.h>
#include <PY/PY_InterpreterAutoLock.h>
#include <PY/PY_AutoObject.h>

#include <SOP/SOP_Node.h>
#include <OP/OP_Director.h>
#include <UT/UT_SysSpecific.h>
#include <GEO/GEO_PrimPart.h>
#include <HOM/HOM_Module.h>

#include <set>
#include <sstream>

#include <prtio/prt_ofstream.hpp>

using namespace prtio;

template <class T>
struct bound_attribute{
    GA_ROAttributeRef attr;
    T* data;
    int count;

    bound_attribute() : data(NULL), count(0)
    {}

    ~bound_attribute(){
        if( data )
            delete data;
    }
};

typedef std::pair<data_types::enum_t, std::size_t> channel_type;

static void exportParticlesDetail(const GU_Detail* gdp,
                                  const std::string& filePath,
                                  const std::map<std::string,
                                  channel_type>& desiredChannels )
{
    if( gdp->getParticleCount() <= 0 )
        return;
	
    prt_ofstream ostream;

    static std::map<std::string, std::string> s_reservedChannels;
    if( s_reservedChannels.empty() ){
        s_reservedChannels[ gdp->getStdAttributeName(GEO_ATTRIBUTE_NORMAL) ] = "Normal";
        s_reservedChannels[ gdp->getStdAttributeName(GEO_ATTRIBUTE_TEXTURE) ] = "TextureCoord";
        s_reservedChannels[ gdp->getStdAttributeName(GEO_ATTRIBUTE_VELOCITY) ] = "Velocity";
        s_reservedChannels[ gdp->getStdAttributeName(GEO_ATTRIBUTE_DIFFUSE) ] = "Color";
        //s_reservedChannels[ gdp->getStdAttributeName(GEO_ATTRIBUTE_ALPHA) ] = "Density";
        //s_reservedChannels[ gdp->getStdAttributeName(GEO_ATTRIBUTE_MASS) ] = "Density";
        s_reservedChannels[ gdp->getStdAttributeName(GEO_ATTRIBUTE_LIFE) ] = "";
        s_reservedChannels[ gdp->getStdAttributeName(GEO_ATTRIBUTE_ID) ] = "ID";
        s_reservedChannels[ gdp->getStdAttributeName(GEO_ATTRIBUTE_PSCALE) ] = "Scale";
        s_reservedChannels[ "accel" ] = "Acceleration";
    }

    float posVal[3];
    float lifeVal[2];
	
    ostream.bind( "Position", posVal, 3 );

    //We handle the life channel in a special manner
    GA_ROAttributeRef lifeAttrib = gdp->findPointAttribute(gdp->getStdAttributeName(GEO_ATTRIBUTE_LIFE));
    if( lifeAttrib.isValid() ){
        std::map<std::string,channel_type>::const_iterator it;
		
        it = desiredChannels.find( "Age" );
        if( it != desiredChannels.end() && it->second.second == 1 )
            ostream.bind( "Age", &lifeVal[0], 1, it->second.first );
        else if( desiredChannels.empty() )
            ostream.bind( "Age", &lifeVal[0], 1, prtio::data_types::type_float16 );

        it = desiredChannels.find( "LifeSpan" );
        if( it != desiredChannels.end() && it->second.second == 1 )
            ostream.bind( "LifeSpan", &lifeVal[1], 1, it->second.first );
        else if( desiredChannels.empty() )
            ostream.bind( "LifeSpan", &lifeVal[1], 1, prtio::data_types::type_float16 );
    }
	
    //Using a deque to prevent the memory from moving around after adding the bound_attribute to the container.
    std::deque< bound_attribute<int> > m_intAttrs;
    std::deque< bound_attribute<float> > m_floatAttrs;
    std::deque< bound_attribute<float> > m_vectorAttrs;

    for (GA_AttributeDict::iterator it = gdp->getAttributes().getDict(GA_ATTRIB_POINT).begin(GA_SCOPE_PUBLIC);
         !it.atEnd(); ++it)
    {
        GA_Attribute            *node = it.attrib();

        std::string channelName = node->getName();

        //Translate special names
        std::map<std::string,std::string>::const_iterator it = s_reservedChannels.find( channelName );
        if( it != s_reservedChannels.end() ){
            //If its empty, that means we reserve some sort of special handling.
            if( it->second.empty() )
                continue;
            channelName = it->second;
        }
		
        //Skip channels that aren't on the list.
        std::map<std::string,channel_type>::const_iterator itChannel = desiredChannels.find( channelName );
        bool channelIsDesired = (itChannel != desiredChannels.end());
		
        if( !desiredChannels.empty() && !channelIsDesired )
            continue;
			
        prtio::data_types::enum_t type;
		
        //I add the new item to the deque, THEN initialize it since a deque will not move the object around and this allows
        //me to allocate the float array and not have to worry about the object getting deleted too early.
        switch( node->getStorageClass() ){
        case GA_STORECLASS_FLOAT:
            if (node->getTupleSize()==3)
            {
                m_vectorAttrs.push_back( bound_attribute<float>() );
                m_vectorAttrs.back().attr =	gdp->findPointAttribute(node->getName());
                m_vectorAttrs.back().count = node->getTupleSize();
                m_vectorAttrs.back().data = new float[m_vectorAttrs.back().count];

                type = prtio::data_types::type_float16;
                if( channelIsDesired ){
                    type = itChannel->second.first;
                    if( itChannel->second.second != m_vectorAttrs.back().count )
                        continue;
                }

                ostream.bind( channelName, m_vectorAttrs.back().data, m_vectorAttrs.back().count, type );

            }
            else
            {
                m_floatAttrs.push_back( bound_attribute<float>() );
                m_floatAttrs.back().attr =	gdp->findPointAttribute(node->getName());
                m_floatAttrs.back().count = node->getTupleSize();
                m_floatAttrs.back().data = new float[m_floatAttrs.back().count];

                type = prtio::data_types::type_float16;
                if( channelIsDesired ){
                    type = itChannel->second.first;
                    if( itChannel->second.second != m_floatAttrs.back().count )
                        continue;
                }

                ostream.bind( channelName, m_floatAttrs.back().data, m_floatAttrs.back().count, type );
            }
            break;
        case GA_STORECLASS_INT:
            m_intAttrs.push_back( bound_attribute<int>() );
            m_intAttrs.back().attr =	gdp->findPointAttribute(node->getName());
            m_intAttrs.back().count = node->getTupleSize();
            m_intAttrs.back().data = new int[m_intAttrs.back().count];

            type = prtio::data_types::type_int32;
            if( channelIsDesired ){
                type = itChannel->second.first;
                if( itChannel->second.second != m_intAttrs.back().count )
                    continue;
            }

            ostream.bind( channelName, m_intAttrs.back().data, m_intAttrs.back().count, type );
            break;
        default:
            break;
        }
    }

    try{
        ostream.open( filePath );
    }catch( const std::ios::failure& e ){
        std::cerr << e.what() << std::endl;

        throw HOM_OperationFailed( "Failed to open the file" );
    }

    GEO_PrimParticle* partPrim = (GEO_PrimParticle*)gdp->primitives().head(GEO_PrimTypeCompat::GEOPRIMPART);
    while( partPrim ){
        GA_Size numVertices = partPrim->getVertexCount();
        for ( GA_Size verticesIndex = 0; verticesIndex < numVertices; ++verticesIndex)
        {
            GEO_Vertex vertex = partPrim->getVertexElement(verticesIndex);
            GEO_Vertex* prt = &vertex;
            UT_Vector4 p = prt->getPos();
			
            posVal[0] =  p.x() / p.w();
            posVal[1] = p.y() / p.w();
            posVal[2] = p.z() / p.w();

            GEO_Point* pt = prt->getPt();

            //TODO: Convert this into appropriate time values. Is it seconds or frames or what?!
            if( lifeAttrib.isValid() )
                pt->get( lifeAttrib, lifeVal, 2 );

            for( std::deque< bound_attribute<float> >::iterator it = m_floatAttrs.begin(), itEnd = m_floatAttrs.end(); it != itEnd; ++it )
                pt->get( it->attr, it->data, it->count );

            for( std::deque< bound_attribute<float> >::iterator it = m_vectorAttrs.begin(), itEnd = m_vectorAttrs.end(); it != itEnd; ++it ){
                pt->get( it->attr, it->data, it->count );
				
                //TODO: Optionally transform into some consistent world space for PRT files.
            }

            for( std::deque< bound_attribute<int> >::iterator it = m_intAttrs.begin(), itEnd = m_intAttrs.end(); it != itEnd; ++it )
                pt->get( it->attr, it->data, it->count );

            ostream.write_next_particle();

        }

        partPrim = (GEO_PrimParticle*)gdp->primitives().next(partPrim, GEO_PrimTypeCompat::GEOPRIMPART);
    }

    ostream.close();
}

static void exportParticles(const char *node_path, const char *filePath, const std::map<std::string, channel_type>& channels )
{
    OP_Node *op_node = OPgetDirector()->findNode(node_path);
    if (!op_node)
        throw HOM_OperationFailed("Internal error (could not find node)");

    float t = HOM().time();
	
    SOP_Node* sopNode = CAST_SOPNODE(op_node);
    if( !sopNode )
        return;

    // Get our parent.
    OP_Node *parent_node = sopNode->getParent();
    
    // Store the cooking status of our parent node.
    bool was_cooking = false;
    if(parent_node)
    {
        was_cooking = parent_node->isCookingRender();
        parent_node->setCookingRender(true);
    }

    // Create a context with the time we want the geometry at.
    OP_Context  context(t);
    // Get a handle to the geometry.
    GU_DetailHandle gd_handle = sopNode->getCookedGeoHandle(context);

    // Restore the cooking flag, if we changed it.
    if(parent_node)
        parent_node->setCookingRender(was_cooking);

    // Check if we have a valid detail handle.
    if( gd_handle.isNull() )
        return;

    // Lock it for reading.
    GU_DetailHandleAutoReadLock gd_lock(gd_handle);

    // Finally, get at the actual GU_Detail.
    const GU_Detail* gdp = gd_lock.getGdp();
	
    exportParticlesDetail( gdp, filePath, channels );
}

static PY_PyObject* createHouException( const char *exception_class_name,
                                        const char *instance_message,
                                        PY_PyObject *&exception_class)
{
    // Create an instance of the given exception class from the hou
    // module, passing the instance message into the exeption class's
    // __init__ method.  This function returns a new exception instance, or
    // NULL if an error occurred.  The class is returned in exception_class
    // and is a borrowed reference.
    exception_class = NULL;

    // Note that a PY_AutoObject class is just a wrapper around a
    // PY_PyObject pointer that will call PY_Py_XDECREF when the it's destroyed.
    // We use it for Python API functions that return new object instances.
    // Because this HDK extension is installed after the hou module is
    // imported, we can be sure that we can be sure hou_module won't be null.
    PY_AutoObject hou_module(PY_PyImport_ImportModule("hou"));

    // Look up the exception by name in the module's dictionary.  Note that
    // PY_PyModule_GetDict returns a borrowed reference and that it never
    // returns NULL.  PY_PyDict_GetItemString also returns a borrowed
    // reference.
    PY_PyObject *hou_module_dict = PY_PyModule_GetDict(hou_module);
    exception_class = PY_PyDict_GetItemString(hou_module_dict, exception_class_name);

    // PY_PyDict_GetItemString doesn't set a Python exception, so we are careful
    // to set it ourselves if the class name isn't valid.
    if (!exception_class)
    {
        PY_PyErr_SetString(PY_PyExc_RuntimeError(), "Could not find exception class in hou module");
        return NULL;
    }

    // Create an instance of the exception.  First create a tuple containing
    // the arguments to __init__.
    PY_AutoObject args(PY_Py_BuildValue("(s)", instance_message));
    if (!args)
        return NULL;

    return PY_PyObject_Call(exception_class, args, /*kwargs=*/NULL);
}

static PY_PyObject* exportParticles_Wrapper(PY_PyObject *self, PY_PyObject *args)
{
    // This is a wrapper that is called from the Python runtime engine.  It
    // translates the Python arguments to C/C++ ones, calls a function to do
    // the actual work, and converts exceptions raised by that function into
    // Python exceptions.
    //
    // Note that you could also use swig to automatically generate wrapper
    // functions like this.
    //
    // Since this function is called from the Python runtime engine, we
    // don't need to manually acquire the Python global interpreter lock (GIL).

    // First extract the arguments: a string and a string.
    //const char *node_path;
    PY_PyObject* node;
    PY_PyObject* channelList = NULL;
    const char *file_path;

    if( !PY_PyArg_ParseTuple( args, "Os|O", &node, &file_path, &channelList) )
        return NULL;

    PY_AutoObject nodePath = PY_PyObject_CallMethod(node, "path", NULL);
    if( !nodePath )
        return NULL;
	
    std::map< std::string, channel_type > channels;
	
    if( channelList != NULL ){
        try{
            PY_Py_ssize_t listLen = PY_PySequence_Size( channelList );
			
            for( PY_Py_ssize_t i = 0; i < listLen; ++i ){
                PY_PyObject* p = PY_PySequence_GetItem( channelList, i );
				
                char* curString = p ? PY_PyString_AsString( p ) : NULL;
                if( !curString )
                    return NULL;
				
                const char *nameStart = curString, *nameEnd = curString;
                while( *nameEnd != '\0' && std::isalnum(*nameEnd) )
                    ++nameEnd;
				
                std::string name(nameStart, nameEnd);
				
                channel_type type = prtio::data_types::parse_data_type( nameEnd );
				
                channels[name] = type;
            }
        }catch( const std::exception& e ){
            PY_PyErr_SetString(PY_PyExc_TypeError(), e.what());
            return NULL;
        }
    }

    // Now call ObjNode_setSelectable to do the actual work, taking care
    // of the locking and exception handling here.
    try
    {
        // If this code is called from a thread other than the main thread,
        // creating a HOM_AutoLock instance will lock, waiting until Houdini
        // is sitting idle in its event loop.  This way, we can be sure that
        // any code that accesses Houdini's internal state is threadsafe.
        HOM_AutoLock hom_lock;

        // Call the wrapped function to do the actual work.
        exportParticles(PY_PyString_AsString(nodePath), file_path, channels);

        // Return PY_Py_None to indicate that no error occurred.  If your
        // wrapped function returns a value, you'll need to convert it into
        // a Python object here.
        return PY_Py_None();
    }
    catch (HOM_Error &error)
    {
        cerr << error.instanceMessage() << std::endl;

        // The exceptions used by the hou module are subclasses of HOM_Error
        // (and can be found in HOM_Errors.h).  We use RTTI to get the class
        // name, remove the "HOM_" prefix, and look up the corresponding
        // exception class in the hou Python module.
        std::string exception_class_name = UTunmangleClassNameFromTypeIdName(typeid(error).name());
        if (exception_class_name.find("HOM_") == 0)
            exception_class_name = exception_class_name.substr(4);

        // Note that a PY_AutoObject class is just a wrapper around a
        // PY_PyObject pointer that will call PY_Py_XDECREF when the it's
        // destroyed.
        PY_PyObject* exception_class;
        PY_AutoObject exception_instance(createHouException(exception_class_name.c_str(),
                                                            error.instanceMessage().c_str(),exception_class));
        if (!exception_instance)
            return NULL;

        // Set the exception and return NULL so Python knows an exception was
        // raised.
        PY_PyErr_SetObject(exception_class, exception_instance);
        return NULL;
    }
}

void HOMextendLibrary()
{
    // This function installs the C++ HOM extension.  When the hou module is
    // first imported, Houdini will call functions named HOMextendLibrary in
    // HDK dso's.  This function is declared in an extern "C" in HOM_Module.h.

    {
        // A PY_InterpreterAutoLock will grab the Python global interpreter
        // lock (GIL).  It's important that we have the GIL before making
        // any calls into the Python API.
        PY_InterpreterAutoLock interpreter_auto_lock;

        // We'll create a new module named "_hom_extensions", and add functions
        // to it.  We don't give a docstring here because it's given in the
        // Python implementation below.
        static PY_PyMethodDef krakatoaModule[] = {
            {"exportParticles", exportParticles_Wrapper, PY_METH_VARARGS(), ""},
            { NULL, NULL, 0, NULL }
        };

        PY_Py_InitModule("Krakatoa", krakatoaModule);
    }
}
