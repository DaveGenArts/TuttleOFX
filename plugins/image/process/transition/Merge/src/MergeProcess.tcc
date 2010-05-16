#include "gil/toolbox/ViewsMerging.hpp"
#include <boost/gil/extension/toolbox/hsl.hpp>
#include <tuttle/common/math/rectOp.hpp>
#include <tuttle/plugin/ImageGilProcessor.hpp>
#include <tuttle/plugin/PluginException.hpp>

#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>
#include <boost/gil/gil_all.hpp>

namespace tuttle {
namespace plugin {
namespace merge {


template<class View, class Functor>
MergeProcess<View, Functor>::MergeProcess( MergePlugin& instance )
	: ImageGilProcessor<View>( instance ),
	_plugin( instance )
{}

template<class View, class Functor>
void MergeProcess<View, Functor>::setup( const OFX::RenderArguments& args )
{
	ImageGilProcessor<View>::setup( args );

	// sources view
	// clip A
	_srcA.reset( _plugin.getSrcClipA()->fetchImage( args.time ) );
	if( !_srcA.get() )
		throw( ImageNotReadyException() );
	if( _srcA->getRowBytes( ) <= 0 )
		throw( WrongRowBytesException( ) );
	this->_srcViewA = this->getView( _srcA.get(), _plugin.getSrcClipA()->getPixelRod(args.time) );
//	_srcPixelRodA = _srcA->getRegionOfDefinition(); // bug in nuke, returns bounds
	_srcPixelRodA = _plugin.getSrcClipA()->getPixelRod(args.time);
	// clip B
	_srcB.reset( _plugin.getSrcClipB()->fetchImage( args.time ) );
	if( !_srcB.get() )
		throw( ImageNotReadyException() );
	if( _srcB->getRowBytes( ) <= 0 )
		throw( WrongRowBytesException( ) );
	this->_srcViewB = this->getView( _srcB.get(), _plugin.getSrcClipB()->getPixelRod(args.time) );
//	_srcPixelRodB = _srcB->getRegionOfDefinition(); // bug in nuke, returns bounds
	_srcPixelRodB = _plugin.getSrcClipB()->getPixelRod(args.time);

	// Make sure bit depths are the same
	if( _srcA->getPixelDepth() != this->_dst->getPixelDepth() ||
	    _srcB->getPixelDepth() != this->_dst->getPixelDepth() ||
		_srcA->getPixelComponents() != this->_dst->getPixelComponents() ||
	    _srcB->getPixelComponents() != this->_dst->getPixelComponents() )
	{
		throw( BitDepthMismatchException() );
	}
}

/**
 * @brief Function called by rendering thread each time a process must be done.
 * @param[in] procWindowRoW  Processing window in RoW
 */
template<class View, class Functor>
void MergeProcess<View, Functor>::multiThreadProcessImages( const OfxRectI& procWindowRoW )
{
	using namespace boost::gil;
	OfxPointI procWindowSize = { procWindowRoW.x2 - procWindowRoW.x1,
							     procWindowRoW.y2 - procWindowRoW.y1 };

	View srcViewA = subimage_view( this->_srcViewA,
							       procWindowRoW.x1 - _srcPixelRodA.x1,
							       procWindowRoW.y1 - _srcPixelRodA.y1,
							       procWindowSize.x,
							       procWindowSize.y );
	View srcViewB = subimage_view( this->_srcViewB,
							       procWindowRoW.x1 - _srcPixelRodB.x1,
							       procWindowRoW.y1 - _srcPixelRodB.y1,
							       procWindowSize.x,
							       procWindowSize.y );
	View dstView = subimage_view( this->_dstView,
							      procWindowRoW.x1 - this->_dstPixelRod.x1,
							      procWindowRoW.y1 - this->_dstPixelRod.y1,
							      procWindowSize.x,
							      procWindowSize.y );
	merge_pixels( srcViewA, srcViewB, dstView, Functor() );
}

}
}
}