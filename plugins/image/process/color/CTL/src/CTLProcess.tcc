#include "CTLAlgorithm.hpp"
#include "CTLProcess.hpp"

#include <IlmCtl/CtlMessage.h>
#include <IlmCtlSimd/CtlSimdInterpreter.h>
#include <Iex.h>


namespace tuttle {
namespace plugin {
namespace ctl {

namespace {
CTLPlugin* ctlPlugin;

void ctlMessageOutput( const std::string& message )
{
	if( ctlPlugin )
	{
		ctlPlugin->sendMessage( OFX::Message::eMessageMessage, "CTL message", message );
	}
}

}

template<class View>
CTLProcess<View>::CTLProcess( CTLPlugin &effect )
: ImageGilFilterProcessor<View>( effect )
, _plugin( effect )
{
	this->setNoMultiThreading();
}

template<class View>
void CTLProcess<View>::setup( const OFX::RenderArguments& args )
{
	ImageGilFilterProcessor<View>::setup( args );
	_params = _plugin.getProcessParams( args.renderScale );

	ctlPlugin = &_plugin;
	Ctl::setMessageOutputFunction( ctlMessageOutput );
}

/**
 * @brief Function called by rendering thread each time a process must be done.
 * @param[in] procWindowRoW  Processing window
 */
template<class View>
void CTLProcess<View>::multiThreadProcessImages( const OfxRectI& procWindowRoW )
{
	using namespace boost::gil;
	OfxRectI procWindowOutput = this->translateRoWToOutputClipCoordinates( procWindowRoW );
	Ctl::SimdInterpreter interpreter;
	
	for( int y = procWindowOutput.y1;
			 y < procWindowOutput.y2;
			 ++y )
	{
		typename View::x_iterator src_it = this->_srcView.x_at( procWindowOutput.x1, y );
		typename View::x_iterator dst_it = this->_dstView.x_at( procWindowOutput.x1, y );
		for( int x = procWindowOutput.x1;
			 x < procWindowOutput.x2;
			 ++x, ++src_it, ++dst_it )
		{
			(*dst_it) = (*src_it);
		}
		if( this->progressForward() )
			return;
	}
	/*
	const OfxRectI procWindowSrc = translateRegion( procWindowRoW, this->_srcPixelRod );
	OfxPointI procWindowSize = { procWindowRoW.x2 - procWindowRoW.x1,
							     procWindowRoW.y2 - procWindowRoW.y1 };
	View src = subimage_view( this->_srcView, procWindowSrc.x1, procWindowSrc.y1,
							                  procWindowSize.x, procWindowSize.y );
	View dst = subimage_view( this->_dstView, procWindowOutput.x1, procWindowOutput.y1,
							                  procWindowSize.x, procWindowSize.y );
	copy_pixels( src, dst );
	*/

}

}
}
}