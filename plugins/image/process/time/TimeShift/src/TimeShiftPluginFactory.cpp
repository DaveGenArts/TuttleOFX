#include "TimeShiftPluginFactory.hpp"
#include "TimeShiftPlugin.hpp"
#include "TimeShiftDefinitions.hpp"

#include <ofxsImageEffect.h>

namespace tuttle {
namespace plugin {
namespace timeShift {

/**
 * @brief Function called to describe the plugin main features.
 * @param[in, out]   desc     Effect descriptor
 */
void TimeShiftPluginFactory::describe( OFX::ImageEffectDescriptor& desc )
{
	desc.setLabels( "TuttleTimeShift", "TimeShift",
	                "TimeShift" );
	desc.setPluginGrouping( "tuttle/image/process/time" );

	// add the supported contexts
	desc.addSupportedContext( OFX::eContextFilter );
	desc.addSupportedContext( OFX::eContextGeneral );

	// add supported pixel depths
	desc.addSupportedBitDepth( OFX::eBitDepthUByte );
	desc.addSupportedBitDepth( OFX::eBitDepthUShort );
	desc.addSupportedBitDepth( OFX::eBitDepthFloat );

	// plugin flags
	desc.setTemporalClipAccess( true ); // The identity state change the time
	desc.setSupportsTiles( true );
	desc.setRenderThreadSafety( OFX::eRenderFullySafe );
}

/**
 * @brief Function called to describe the plugin controls and features.
 * @param[in, out]   desc       Effect descriptor
 * @param[in]        context    Application context
 */
void TimeShiftPluginFactory::describeInContext( OFX::ImageEffectDescriptor& desc,
                                                OFX::EContext               context )
{
	OFX::ClipDescriptor* srcClip = desc.defineClip( kOfxImageEffectSimpleSourceClipName );
	srcClip->addSupportedComponent( OFX::ePixelComponentRGBA );
	srcClip->addSupportedComponent( OFX::ePixelComponentRGB );
	srcClip->addSupportedComponent( OFX::ePixelComponentAlpha );
	srcClip->setTemporalClipAccess( true );

	// Create the mandated output clip
	OFX::ClipDescriptor* dstClip = desc.defineClip( kOfxImageEffectOutputClipName );
	dstClip->addSupportedComponent( OFX::ePixelComponentRGBA );
	dstClip->addSupportedComponent( OFX::ePixelComponentRGB );
	dstClip->addSupportedComponent( OFX::ePixelComponentAlpha );

	switch( context )
	{
		case OFX::eContextRetimer:
		{
			OFX::DoubleParamDescriptor* retimerParam = desc.defineDoubleParam( kOfxImageEffectRetimerParamName );
			retimerParam->setLabel( "Source Time" );
			retimerParam->setDefault( 0.0 );
			break;
		}
		default:
		{
			OFX::DoubleParamDescriptor* offset = desc.defineDoubleParam( kParamOffset );
			offset->setLabel( "Offset" );
			offset->setDefault( 0.0 );
			offset->setDisplayRange( -10, 10 );
			break;
		}
	}
}

/**
 * @brief Function called to create a plugin effect instance
 * @param[in] handle  effect handle
 * @param[in] context    Application context
 * @return  plugin instance
 */
OFX::ImageEffect* TimeShiftPluginFactory::createInstance( OfxImageEffectHandle handle,
                                                          OFX::EContext        context )
{
	return new TimeShiftPlugin( handle );
}

}
}
}
