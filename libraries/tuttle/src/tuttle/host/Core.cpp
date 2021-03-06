#include "Core.hpp"

#include <tuttle/host/ofx/OfxhImageEffectPlugin.hpp>
#include <tuttle/host/memory/MemoryPool.hpp>
#include <tuttle/host/memory/MemoryCache.hpp>

#include <tuttle/common/system/system.hpp>

#include <boost/archive/xml_oarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/convenience.hpp>

#ifdef TUTTLE_HOST_WITH_PYTHON_EXPRESSION
	#include <boost/python.hpp>
#endif

#include <stdexcept>
#include <iostream>
#include <fstream>
#include <vector>
#include <cstring> // memset

namespace tuttle {
namespace host {

namespace {
memory::MemoryPool pool;
memory::MemoryCache cache;
}

Core::Core()
	: _imageEffectPluginCache( _host )
	, _memoryPool( pool )
	, _memoryCache( cache )
	, _isPreloaded( false )
{
#ifdef TUTTLE_HOST_WITH_PYTHON_EXPRESSION
	Py_Initialize( );
#endif
	_pluginCache.setCacheVersion( "tuttleV1" );

	// register the image effect cache with the global plugin cache
	_pluginCache.registerAPICache( _imageEffectPluginCache );

	_memoryPool.updateMemoryAuthorizedWithRAM();
	//	preload();
}

Core::~Core()
{}

void Core::preload( const bool useCache )
{
	if( _isPreloaded )
		return;
	
	_isPreloaded = true;
	
#ifndef __WINDOWS__
	//	typedef boost::archive::binary_oarchive OArchive;
	//	typedef boost::archive::binary_iarchive IArchive;
	//	typedef boost::archive::text_oarchive OArchive;
	//	typedef boost::archive::text_iarchive IArchive;
	typedef boost::archive::xml_oarchive OArchive;
	typedef boost::archive::xml_iarchive IArchive;
	
	std::string cacheFile;
	if( useCache )
	{
		cacheFile = (getPreferences().getTuttleHomePath() / "tuttlePluginCacheSerialize.xml").string();
		
		TUTTLE_LOG_DEBUG( TUTTLE_INFO, "plugin cache file = " << cacheFile );

		try
		{
			std::ifstream ifsb( cacheFile.c_str(), std::ios::in );
			if( ifsb.is_open() )
			{
				TUTTLE_LOG_DEBUG( TUTTLE_INFO, "Read plugins cache." );
				IArchive iArchive( ifsb );
				iArchive >> BOOST_SERIALIZATION_NVP( _pluginCache );
				ifsb.close();
			}
		}
		catch( std::exception& e )
		{
			TUTTLE_LOG_ERROR( "Exception when reading cache file (" << e.what()  << ")." );
		}
	}
#endif
	_pluginCache.scanPluginFiles();
#ifndef __WINDOWS__
	if( useCache && _pluginCache.isDirty() )
	{
		// generate unique name for writing
		boost::uuids::random_generator gen;
		boost::uuids::uuid u = gen();
		const std::string tmpCacheFile( cacheFile + ".writing." + boost::uuids::to_string(u) + ".xml" );
		
		TUTTLE_LOG_DEBUG( TUTTLE_INFO, "Write plugins cache " << tmpCacheFile );
		// serialize into a temporary file
		std::ofstream ofsb( tmpCacheFile.c_str(), std::ios::out );
		if( ofsb.is_open() )
		{
			OArchive oArchive( ofsb );
			oArchive << BOOST_SERIALIZATION_NVP( _pluginCache );
			ofsb.close();
			// replace the cache file
			boost::filesystem::rename( tmpCacheFile, cacheFile );
		}
	}
#endif
}

std::ostream& operator<<( std::ostream& os, const Core& v )
{
	os << "Core {" << std::endl;
	os << v.getImageEffectPluginCache();
	os << "}" << std::endl;
	return os;
}

}
}

