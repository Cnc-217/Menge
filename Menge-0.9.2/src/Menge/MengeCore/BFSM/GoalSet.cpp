/*

License

Menge
Copyright ?and trademark ?2012-14 University of North Carolina at Chapel Hill. 
All rights reserved.

Permission to use, copy, modify, and distribute this software and its documentation 
for educational, research, and non-profit purposes, without fee, and without a 
written agreement is hereby granted, provided that the above copyright notice, 
this paragraph, and the following four paragraphs appear in all copies.

This software program and documentation are copyrighted by the University of North 
Carolina at Chapel Hill. The software program and documentation are supplied "as is," 
without any accompanying services from the University of North Carolina at Chapel 
Hill or the authors. The University of North Carolina at Chapel Hill and the 
authors do not warrant that the operation of the program will be uninterrupted 
or error-free. The end-user understands that the program was developed for research 
purposes and is advised not to rely exclusively on the program for any reason.

IN NO EVENT SHALL THE UNIVERSITY OF NORTH CAROLINA AT CHAPEL HILL OR THE AUTHORS 
BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL 
DAMAGES, INCLUDING LOST PROFITS, ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS 
DOCUMENTATION, EVEN IF THE UNIVERSITY OF NORTH CAROLINA AT CHAPEL HILL OR THE 
AUTHORS HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

THE UNIVERSITY OF NORTH CAROLINA AT CHAPEL HILL AND THE AUTHORS SPECIFICALLY 
DISCLAIM ANY WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES 
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE AND ANY STATUTORY WARRANTY 
OF NON-INFRINGEMENT. THE SOFTWARE PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND 
THE UNIVERSITY OF NORTH CAROLINA AT CHAPEL HILL AND THE AUTHORS HAVE NO OBLIGATIONS 
TO PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.

Any questions or comments should be sent to the authors {menge,geom}@cs.unc.edu

*/

#include "MengeCore/BFSM/GoalSet.h"

#include "MengeCore/BFSM/fsmCommon.h"
#include "MengeCore/BFSM/Goals/Goal.h"
#include "MengeCore/Math/consts.h"
#include "MengeCore/Core.h"
#include "MengeCore/BFSM/FSM.h"
#include "MengeCore/BFSM/State.h"
#include "MengeCore/BFSM/GoalSelectors/GoalSelector.h"
#include "MengeCore/Agents/BaseAgent.h"

#include "MengeCore/MatrixMy.h"

#include <cassert>
#include <cmath>
#include <ctime>
using namespace Menge::Olympic;
using namespace std;
namespace Menge {

	namespace BFSM {

		/////////////////////////////////////////////////////////////////////
		//					Implementation of GoalSet
		/////////////////////////////////////////////////////////////////////

		GoalSet::GoalSet():_goals(),_goalIDs(),_totalWeight(0.f),_randVal(0.f,1.f) {
		}

		/////////////////////////////////////////////////////////////////////

		GoalSet::~GoalSet() {
			std::map< size_t, Goal * >::iterator itr = _goals.begin();
			for ( ; itr != _goals.end(); ++itr ) {
				itr->second->destroy();
			}
		}

		/////////////////////////////////////////////////////////////////////

		bool GoalSet::addGoal( size_t id, Goal * goal ) {
			bool valid = false;
			_lock.lockRead();
			if ( _goals.find( id ) == _goals.end() ) {
				valid = true;
				goal->_goalSet = this;
				_goals[ id ] = goal;
				_goalIDs.push_back( id );
				_totalWeight += goal->_weight;
			}
			_lock.releaseRead();
			return valid;
		}

		/////////////////////////////////////////////////////////////////////
		void GoalSet::moveGoals(float time_step) {
			for (auto& id_goal_pair : _goals) {
				Goal& goal = *id_goal_pair.second;
				if (goal.moves()) goal.move(time_step);
			}
		}


		/////////////////////////////////////////////////////////////////////

		Goal * GoalSet::getGoalByID( size_t id ) {
			Goal * goal = 0x0;
			std::map< size_t, Goal * >::const_iterator itr = _goals.find( id );
			if ( itr != _goals.end() && itr->second->hasCapacity() ) {
				goal = itr->second;
			}
			return goal;
		}

		/////////////////////////////////////////////////////////////////////

		Goal * GoalSet::getGoalByIDConcurrent( size_t id ) {
			_lock.lockRead();
			Goal * goal = getGoalByID( id );
			_lock.releaseRead();
			return goal;
		}

		/////////////////////////////////////////////////////////////////////

		Goal * GoalSet::getIthGoal( size_t i ) {
			Goal * goal = 0x0;
			if ( i < _goalIDs.size() ) {
				size_t id = _goalIDs[ i ];
				std::map< size_t, Goal * >::const_iterator itr = _goals.find( id );
				if (itr != _goals.end() && itr->second->hasCapacity() ) {
					goal = itr->second;
				}
			}
			return goal;
		}

		/////////////////////////////////////////////////////////////////////

		Goal * GoalSet::getIthGoalConcurrent( size_t i ) {
			_lock.lockRead();
			Goal * goal = getIthGoal( i );
			_lock.releaseRead();
			return goal;
		}

		/////////////////////////////////////////////////////////////////////

		size_t GoalSet::sizeConcurrent() const {
			_lock.lockRead();
			size_t s = _goalIDs.size();
			_lock.releaseRead();
			return s;
		}

		/////////////////////////////////////////////////////////////////////

		Goal * GoalSet::getRandomGoal() {
			Goal * goal = 0x0;
			const size_t GOAL_COUNT = _goalIDs.size();
			if ( GOAL_COUNT > 0 ) {
				size_t idx = (size_t)( GOAL_COUNT * _randVal.getValue() );
				idx = idx < GOAL_COUNT ? idx : GOAL_COUNT - 1;
				size_t id = _goalIDs[ idx ];
				std::map< size_t, Goal * >::const_iterator itr = _goals.find( id );
				assert( itr != _goals.end() && "Somehow failed to get a random goal" );
				goal = itr->second;
			}
			return goal;
		}

		/////////////////////////////////////////////////////////////////////

		Goal * GoalSet::getRandomWeightedGoal() {
			// TODO: Change this to use _goalIDs as the key interface of available goals
			Goal * tgtGoal = 0x0;
			if ( _goalIDs.size() > 0 ) {
 				const float TGT_WEIGHT = _totalWeight * _randVal.getValue();

				std::map< size_t, Goal * >::const_iterator itr = _goals.find( _goalIDs[ 0 ] );
				assert( itr != _goals.end() && "A goalID does not map to a goal" );
				tgtGoal = itr->second;
				float accumWeight = tgtGoal->_weight;
				for ( size_t i = 1; i < _goalIDs.size(); ++i ) {
					if ( accumWeight > TGT_WEIGHT ) break;
					itr = _goals.find( _goalIDs[ i ] );
					assert( itr != _goals.end() && "A goalID does not map to a goal" );
					tgtGoal = itr->second;
					accumWeight += tgtGoal->_weight;
				}
			}
			return tgtGoal;
		}

		/////////////////////////////////////////////////////////////////////

		void GoalSet::setGoalFull( const Goal * goal ) const {
			size_t i = 0;
			std::map< size_t, Goal * >::const_iterator itr;
			while ( i < _goalIDs.size() ) {
				itr = _goals.find( _goalIDs[ i ] );
				assert( itr != _goals.end() && "A goalID does not map to a goal" );
				const Goal * testGoal = itr->second;
				if ( testGoal == goal ) {
					_totalWeight -= goal->_weight;
					_goalIDs.erase( _goalIDs.begin() + i ); // todo: should this just be itr?
					break;
				} else {
					++i;
				}
			}
		}

		/////////////////////////////////////////////////////////////////////

		void GoalSet::setGoalAvailable( const Goal * goal ) const {
			const size_t GOAL_ID = goal->getID();
			_lock.lockWrite();
			assert( _goals.find( GOAL_ID ) != _goals.end() &&
					"Trying to set a goal available that doesn't belong to the goal set" );
	#ifdef _DEBUG
			bool found = false;
			for ( size_t i = 0; i < _goalIDs.size(); ++i ) {
				if ( _goalIDs[ i ] == GOAL_ID ) {
					found = true;
					break;
				}
			}
			assert( !found && "Trying to reactivate a goal that was never marked unavailable" );
	#endif
			_goalIDs.push_back( GOAL_ID );
			_totalWeight += goal->_weight;
			_lock.releaseWrite();
		}

		/////////////////////////////////////////////////////////////////////

		Goal* GoalSet::getGoalFromMatrix(const Agents::BaseAgent* agent) {
			// TODO: Change this to use _goalIDs as the key interface of available goals
			Goal* tgtGoal = 0x0;
			if (PROJECTNAME == BUSINESSREALITY) {
				if (SIM_TIME > 0) {
					int shopIDNow = ACTIVE_FSM->getNode(0)->getGoal(agent->_id)->_id;//agent当前的goal的id
					float weight = BusinessReality::ProbGoals->_sumWeightMatrix->Point(agent->_class, shopIDNow);
					LARGE_INTEGER seed;
					QueryPerformanceFrequency(&seed);
					QueryPerformanceCounter(&seed);
					srand(seed.QuadPart);//取硬件计时器，精度更高
					float TGT_WEIGHT = weight * (rand() % 100 * 0.01);//_totalWeight
					std::map< size_t, Goal* >::const_iterator itr = _goals.find(_goalIDs[0]);
					assert(itr != _goals.end() && "A goalID does not map to a goal");
					tgtGoal = itr->second;
					float accumWeight = BusinessReality::ProbGoals->Point(agent->_class, shopIDNow, tgtGoal->_id / 10, tgtGoal->_id % 10);//这里用矩阵
					for (size_t i = 1; i < _goalIDs.size(); ++i) {
						if (accumWeight > TGT_WEIGHT) break;
						itr = _goals.find(_goalIDs[i]);
						assert(itr != _goals.end() && "A goalID does not map to a goal");
						tgtGoal = itr->second;
						accumWeight += BusinessReality::ProbGoals->Point(agent->_class, shopIDNow, tgtGoal->_id / 10, tgtGoal->_id % 10);
					}
				}

				else {
					tgtGoal = GoalSet::getRandomWeightedGoal();
				}
				
			
				return tgtGoal;
			}

			else if (PROJECTNAME == THEMEPARK || PROJECTNAME == OLYMPIC) {
				if (SIM_TIME > 0) {
					int goalIDNow = ACTIVE_FSM->getNode("tour")->getGoal(agent->_id)->_id;//agent当前的goal的id
					int goalSetNow = shopInfo[goalIDNow].goalSet;//agent当前的goalset的id	
					//weight权重和的值如下
					float weight = BaseScene::ProbMatrix->_sumWeight->at(goalIDNow);
					LARGE_INTEGER seed;
					QueryPerformanceFrequency(&seed);
					QueryPerformanceCounter(&seed);
					srand(seed.QuadPart);//取硬件计时器，精度更高
					float TGT_WEIGHT = weight * ((rand() % 100) * 0.01);//概率和为weight
					float accumWeight = 0;
					std::map< size_t, Goal* >::const_iterator itr;
					for (size_t i = 0; i < _goalIDs.size(); i++) {
						itr = _goals.find(_goalIDs[i]);
						tgtGoal = itr->second;
						accumWeight += BaseScene::ProbMatrix->Point(goalIDNow, (tgtGoal->_id));
						if (accumWeight > TGT_WEIGHT) break;
					}
					if (agent->_id == 1)
						cout << "about goal  " << goalIDNow << "+" << goalSetNow << "+" << tgtGoal->_id << "+" << shopInfo[tgtGoal->_id].goalSet << endl;
				}
				else {
					//第一次选取目标点必定是选目标点0
					tgtGoal = getGoalByID(34);
				}
				return tgtGoal;


			}

			//default project
			else {
				if (SIM_TIME > 0) {
					int goalIDNow = ACTIVE_FSM->getNode("tour")->getGoal(agent->_id)->_id;//agent当前的goal的id
					//如果概率和不为1，weight和的值如下 
					float weight = BaseScene::ProbMatrix->_sumWeight->at(goalIDNow);
					LARGE_INTEGER seed;
					QueryPerformanceFrequency(&seed);
					QueryPerformanceCounter(&seed);
					srand(seed.QuadPart);//取硬件计时器，精度更高
					float TGT_WEIGHT = weight * ((rand() % 100) * 0.01);//概率和为1

					//float accumWeight = BaseScene::ProbMatrix->Point(goalIDNow-1, (tgtGoal->_id)-1);
					float accumWeight = 0;
					std::map< size_t, Goal* >::const_iterator itr;
					for (size_t i = 0; i < _goalIDs.size(); i++) {
						itr = _goals.find(_goalIDs[i]);
						tgtGoal = itr->second;
						accumWeight += BaseScene::ProbMatrix->Point(goalIDNow, (tgtGoal->_id));
						if (accumWeight > TGT_WEIGHT) break;
					}

				}

				else {
					//第一次选取目标点为随机选取
					tgtGoal = GoalSet::getRandomWeightedGoal();
				}
			}
		
		}

		Goal* GoalSet::getGoalNearly(const Agents::BaseAgent* agent)//在当前goalset下选择目标店铺
		{
			if (SIM_TIME > 0)
			{
				Goal* tgtGoal = 0x0;
				int goalIDNow = ACTIVE_FSM->getNode("tour")->getGoal(agent->_id)->_id;//agent当前的goal的id
				int goalSetNow = shopInfo[goalIDNow].goalSet;//agent当前的goalset的id				
				int goalTypeNow = shopInfo[goalIDNow].type;//agent当前的goal的类型0吃1喝2玩3噪点4出入口
				vector<int>goalSet;//当前goalset内的goal的id
				for (int i = 0; i < 36; i++)
				{
					if (shopInfo[i].goalSet == goalSetNow)
						goalSet.push_back(i);
					if (goalSet.size() == goalSetInfo[goalSetNow])
						break;
				}
				int i = 0;
				float x = 0;
				while (i < goalSetInfo[goalSetNow])
				{
					x += BaseScene::ProbMatrix->Point(goalIDNow, goalSet[i]);
					i++;
				}
				//weight权重和的值如下
				float weight = BaseScene::ProbMatrix->_sumWeight->at(goalIDNow);
				float y = (weight - x * 5) / (weight - x);
				float k = weight / x;
				if (agent->_id == 1)
					cout << "canshu: " << x << "+" << y << "+" << k << endl;

				LARGE_INTEGER seed;
				QueryPerformanceFrequency(&seed);
				QueryPerformanceCounter(&seed);
				srand(seed.QuadPart);//取硬件计时器，精度更高
				float TGT_WEIGHT = weight * ((rand() % 100) * 0.01);//概率和为weight
				float accumWeight = 0;
				std::map< size_t, Goal* >::const_iterator itr;
				for (size_t i = 0; i < _goalIDs.size(); i++) {
					itr = _goals.find(_goalIDs[i]);
					tgtGoal = itr->second;
					if (shopInfo[tgtGoal->_id].goalSet == goalSetNow)
						accumWeight += BaseScene::ProbMatrix->Point(goalIDNow, (tgtGoal->_id)) * k;
					/*else
						accumWeight += BaseScene::ProbMatrix->Point(goalIDNow, (tgtGoal->_id)) * 0;*/
					if (accumWeight > TGT_WEIGHT) break;
				}
				if (agent->_id == 1)
					cout <<"about goal  "<< goalIDNow << "+" << goalSetNow << "+" << tgtGoal->_id << "+" << shopInfo[tgtGoal->_id].goalSet << endl;
				return tgtGoal;
			}
			else
				return getGoalByID(34);
			
		}

		Goal* GoalSet::getGoalNearlyNew(const Agents::BaseAgent* agent)
		{
			if (SIM_TIME > 0)
			{
				Goal* tgtGoal = 0x0;
				int goalIDNow = ACTIVE_FSM->getNode("tour")->getGoal(agent->_id)->_id;//agent当前的goal的id
				int goalSetNow = shopInfo[goalIDNow].goalSet;//agent当前的goalset的id				
				int goalTypeNow = shopInfo[goalIDNow].type;//agent当前的goal的类型0吃1买2玩3噪点4出入口
				int sameSetGoalNum = shopInfo[goalIDNow].sameSetGoalNum;
				unsigned seed = time(0);
				srand(seed);
				if (goalTypeNow == 1 || goalTypeNow == 3)
				{
					vector<int>goalSet;//挑选出当前goalset内的goal的id
					for (int i = 0; i < 36; i++)
					{
						if (shopInfo[i].goalSet == goalSetNow)//如果goalsetID相同
							if (i == goalIDNow && ((rand() % 100) * 0.01) < 0.9)//有0.9的概率不重复进入自己
								continue;
							else
								goalSet.push_back(i);
						if (goalSet.size() == sameSetGoalNum)//如果数量已经够了  就退出
							break;
					}
					
					int randomNumber = rand() % goalSet.size();//取概率
					std::map< size_t, Goal* >::const_iterator itr;
					itr = _goals.find(goalSet[randomNumber]);
					tgtGoal = itr->second;
					if (agent->_id == 1)
						cout << "about goal  " << goalIDNow << "+" << goalSetNow << "+" << tgtGoal->_id << "+" << shopInfo[tgtGoal->_id].goalSet << endl;
					return tgtGoal;
				}
				else
				{
					float weight = BaseScene::ProbMatrix->_sumWeight->at(goalIDNow);
					LARGE_INTEGER seed;
					QueryPerformanceFrequency(&seed);
					QueryPerformanceCounter(&seed);
					srand(seed.QuadPart);//取硬件计时器，精度更高
					float TGT_WEIGHT = weight * ((rand() % 100) * 0.01);//概率和为weight
					float accumWeight = 0;
					std::map< size_t, Goal* >::const_iterator itr;
					for (size_t i = 0; i < _goalIDs.size(); i++) {
						itr = _goals.find(_goalIDs[i]);
						tgtGoal = itr->second;
						accumWeight += BaseScene::ProbMatrix->Point(goalIDNow, (tgtGoal->_id));
						if (accumWeight > TGT_WEIGHT) break;
					}
					if (agent->_id == 1)
						cout << "about goal  " << goalIDNow << "+" << goalSetNow << "+" << tgtGoal->_id << "+" << shopInfo[tgtGoal->_id].goalSet << endl;
					return tgtGoal;
				}
			}
			else
				return getGoalByID(34);
		}
	}	// namespace BFSM 
}	// namespace Menge
