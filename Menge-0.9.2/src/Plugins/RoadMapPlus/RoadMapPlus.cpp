/*


/*!
 *	@file		RoadMapPlus.cpp  
 *	@brief		Plugin for RoadMapPlus
 */

#include "RoadMapPlusConfig.h"
#include "MengeCore/PluginEngine/CorePluginEngine.h"
#include "VelComponentPlus.h"

using Menge::PluginEngine::CorePluginEngine;

extern "C" {
	/*!
	 *	@brief		Retrieves the name of the plug-in.
	 *
	 *	@returns	The name of the plug in.
	 */
	ROADMAPPLUS_API const char * getName() {
		return "roadMapPlus";
	}

	/*!
	 *	@brief		Description of the plug-in.
	 *
	 *	@returns	A description of the plugin.
	 */
	ROADMAPPLUS_API  const char * getDescription() {
		return	"Utilities for " \
			"including the following:\n"\
			"\tModifier \"roadMapPlus\" - . ";
	}

	/*!
	 *	@brief		Registers the plug-in with the PluginEngine
	 *
	 *	@param		engine		A pointer to the plugin engine.
	 */
	ROADMAPPLUS_API void registerCorePlugin( CorePluginEngine * engine ) {
		engine->registerVelCompFactory( new ROAD::VelComponetPlusFactory() );
	}
}
