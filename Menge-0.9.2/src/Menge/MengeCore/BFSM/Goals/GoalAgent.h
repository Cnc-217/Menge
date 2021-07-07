/*
 Menge Crowd Simulation Framework
 Copyright and trademark 2019 Sean Curtis
 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at
	http://www.apache.org/licenses/LICENSE-2.0
 or
	LICENSE.txt in the root of the Menge repository.
 Any questions or comments should be sent to the authors menge@cs.unc.edu
 <http://gamma.cs.unc.edu/Menge/>
*/

/*!
 @file    GoalAgent.h
 @brief   The definition of a BFSM goal that moves towards a agent.
 */

#pragma once

#include "MengeCore/BFSM/Goals/Goal.h"
#include "MengeCore/BFSM/Goals/GoalFactory.h"
#include "MengeCore/BFSM/fsmCommon.h"
#include "MengeCore/CoreConfig.h"

namespace Menge {
	namespace BFSM {

		
		class MENGE_API AgentGoal final : public Goal{
		 public:
			 // Inherits docs from Goal::getStringId().
			 std::string getStringId() const final { return NAME; }

			 // Inherits docs from Goal::moves().
			 bool moves() const final { return true; }

			 // Inherits docs from Goal::move().
			 void move(float time_step) final;


			 /** @brief  Reports the position of the origin of the geometry frame, Go, measured and
						 expressed in the world frame.
			  */
			 const Math::Vector2& origin() const { return _p_WGo; }

			 /*! The unique identifier used to register this type with run-time components. */
			 static const std::string NAME;

			private:
			 friend class AgentGoalFactory;

			 // See documentation in Goal::worldToGeometry();
			 Math::Vector2 worldToGeometry(const Math::Vector2& p_WQ) const final {
				 // This transform is translation only, because the goal doesn't change orientation.
				 return p_WQ - _p_WGo;
			 }

			 // See documentation in Goal::geometryToWorld();
			 Math::Vector2 geometryToWorld(const Math::Vector2& p_GQ) const final {
				 // This transform is translation only, because the goal doesn't change orientation.
				 return p_GQ + _p_WGo;
			 }

			 // The "position" (or origin) of the geometry's frame measured and expressed in the world frame.
			 Math::Vector2 _p_WGo;

			 size_t _leaderID;
		};

		/*!
		 @brief    Factory for the PathGoal.
		 */
		class MENGE_API AgentGoalFactory : public GoalFactory {
		public:
			AgentGoalFactory();

			// Inhertis docs from GoalFactory::name().
			const char* name() const override { return AgentGoal::NAME.c_str(); }

			// Inherits docs from GoalFactory::description().
			const char* description() const override {
				return "An agent goal consisting of a specified geometry shape that moves along a fixed path";
			};

		protected:
			/*!
			 @brief    Create an instance of this class's goal.
			 @returns    A pointer to a newly instantiated Goal class.
			 */
			Goal* instance() const { return new AgentGoal(); }

			// Inherits docs from GoalFactory::setFromXML().
			bool setFromXML(Goal* goal, TiXmlElement* node, const std::string& behaveFldr) const override;

			/*!
			 @brief    The identifier for the "shape" stringattribute.
			 */
			size_t _shapeId;

			size_t _leaderId;
		};
	}  // namespace BFSM
}  // namespace Menge