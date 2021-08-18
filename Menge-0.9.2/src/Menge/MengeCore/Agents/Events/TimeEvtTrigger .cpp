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

#include "MengeCore/Agents/Events/TimeEvtTrigger.h"

#include "MengeCore/Agents/Events/EventSystem.h"
#include "MengeCore/BFSM/FSM.h"
#include "MengeCore/BFSM/State.h"

#include "MengeCore/BFSM/Transitions/TargetProb.h"
#include "MengeCore/BFSM/Goals/Goal.h"
#include "MengeCore/BFSM/GoalSet.h"
#include "MengeCore/BFSM/GoalSelectors/GoalSelector.h"
#include "MengeCore/BFSM/GoalSelectors/GoalSelectorAlgorithm.h"
#include "MengeCore/MatrixMy.h"
#include "MengeCore/BFSM/Transitions/Transition.h"
#include "MengeCore/Agents/SimulatorInterface.h"
#include "MengeCore/Agents/BaseAgent.h"
#include "MengeCore/Scene/Business.h"
#include "MengeCore/Socket.h"

#include "MengeCore/Core.h"
#include "MengeVis/Viewer/GLViewer.h"
#include <winsock.h>
#pragma comment(lib, "ws2_32.lib")

#include <sstream>

//extern MengeVis::Viewer::GLViewer* SimViewer;

namespace Menge {

	using namespace std;
	using namespace Menge::BFSM;

	
	//extern const MatrixDim2* Business::ProbStatesNormal;
	//extern const MatrixDim2* Business::ProbStatesCoupon;
	//extern const MatrixDim3* Business::ProbGoalsNormal;
	//extern const MatrixDim3* Business::ProbGoalsCoupon;
	extern char* Business::dataInit();
	extern int* Socket::socketGetCouponBusiness(char* message);
	extern int* Socket::socketGetCouponBusinessReality(char* message);

	float TimeReachedTrigger::_timeSimulate = 10;
	float TimeReachedTrigger::_lastTimestamp = 0;

	/////////////////////////////////////////////////////////////////////
	//					Implementation of StateEvtTrigger
	/////////////////////////////////////////////////////////////////////

	TimeEvtTrigger::TimeEvtTrigger() : EventTrigger() {
	}

	/////////////////////////////////////////////////////////////////////

	void TimeEvtTrigger::finalize() {
		
	}
	
	/////////////////////////////////////////////////////////////////////
	//					Implementation of StateEvtTriggerFactory
	/////////////////////////////////////////////////////////////////////

	TimeEvtTriggerFactory::TimeEvtTriggerFactory() : EventTriggerFactory() {
		_timeSimulateIndex = _attrSet.addFloatAttribute( "timeSimulate", true, 0.0 );//设置仿真时间
	}


	/////////////////////////////////////////////////////////////////////

	bool TimeEvtTriggerFactory::setFromXML( EventTrigger * trigger, TiXmlElement * node,
											 const std::string & specFldr ) const {
		TimeEvtTrigger* tTrigger = dynamic_cast<TimeEvtTrigger* >( trigger );
		assert(tTrigger != 0x0 &&
				"Trying to set attributes of a state event trigger on an incompatible object" );

		if ( ! EventTriggerFactory::setFromXML(tTrigger, node, specFldr ) ) return false;

		TimeReachedTrigger::_timeSimulate = _attrSet.getFloat(_timeSimulateIndex);

		return true;
	}

	/////////////////////////////////////////////////////////////////////
	//					Implementation of StatePopIncreaseTrigger
	/////////////////////////////////////////////////////////////////////

	// _lastPop is initialized to a ridiculously high number in order to keep
	//	the event from triggering upon initialization.  The first call to
	//	testCondition will bring it back down.
	TimeReachedTrigger::TimeReachedTrigger() : TimeEvtTrigger() {
		TimeReachedTrigger::_timeSimulate = 10;
		TimeReachedTrigger::_lastTimestamp = 0;
	}

	/////////////////////////////////////////////////////////////////////

	void TimeReachedTrigger::finalize() {
		_lastTimestamp = Menge::SIM_TIME;//完成后获取当前的仿真时间
	}

	/////////////////////////////////////////////////////////////////////

	bool TimeReachedTrigger::testCondition() { //检测触发条件
		if ((Menge::SIM_TIME - _lastTimestamp) > _timeSimulate) {
			cout << "trigger condition met at :"<<Menge::SIM_TIME << endl;
			_lastTimestamp = Menge::SIM_TIME;
			MengeVis::SimViewer->_pause = !MengeVis::SimViewer->_pause;
			cout << "pause" << endl;


			if (PROJECTNAME ==BUSINESS || PROJECTNAME == BUSINESSLEARNING) {
				//下面需要0.初始化数据 1.socket发送数据 2.socket接收python动作 3.根据动作调用函数改变概率矩阵 4.刷新fsm概率
				//0.初始化需要发送的数据：各店铺人数
				//char* message = Business::dataInit();
				char* message = Business::dataInit();

				//1.socket发送数据 2.接收数据
				int* res = Socket::socketGetCouponBusiness(message);
				memset(message, 0, sizeof(message) / sizeof(char));
				

				//3.根据收到的动作，调用函数改变概率矩阵
				
				//[[0,0,1],[0,0,5],3] 第三类店铺，第3类人群，5号店铺，力度3; 注意店铺号id为1-10
				int typeAgent = 0;
				int shopID = 0;
				int strength = 0;
				int typeShop = 0;//0 1 2
				for (int i = 0; i < 7; i++) {
					cout << i << ":" << res[i] << endl;
					if (i < 3 && res[i]!=0) {
						typeShop = i;
					}
					else if (i < 6 && res[i]!=0) {
						typeAgent = i - 3;
						shopID = typeShop * 10 + res[i]-1;
					}
					else {
						strength = res[i];
					}
				}
				cout << "typeAgent" << typeAgent << "shopID"<< shopID<< "strength"<< strength << endl;
				//矩阵概率聚集
				Business::ProbGoalsCoupon->Aggregate(typeAgent, shopID, strength);
				//cout << "ProbStatesNormal and ProbStatesCoupon" << endl;
				//Business::ProbStatesNormal->Show();
				//Business::ProbStatesCoupon->Show();
				//cout << "ProbGoalsNormal and ProbGoalsCoupon" << endl;
				//Business::ProbGoalsNormal->Show();
				Business::ProbGoalsCoupon->Show();
				//根据接收到的信息修改矩阵概率
				Business::MartixToFsm();

				

			}
			else if (PROJECTNAME ==BUSINESSREALITY) {
				//0.初始化需要发送的数据  1.初始化socket 2.发送数据给python 3.接收action 4.将action转换为概率矩阵
				//0 初始化发送的数据
				char* message = BusinessReality::dataInit();
				cout << "message: " << message << endl;

				//1 2 3 初始化socket 发送、接收数据
				int* res = Socket::socketGetCouponBusinessReality(message);
				memset(message, 0, sizeof(message) / sizeof(char));
				cout << "typeAgent: " << res[0] << "  shopID:  " << res[1] << "  strength: " << res[2] << endl;

				//4 将action转换为概率矩阵
				BusinessReality::ProbGoals->Aggregate(res[0], res[1], res[2]);
				//BusinessReality::ProbGoals->Show();
				//根据接收到的信息修改矩阵概率

				

			}
			else {	
			}
			
			//重新开始
			MengeVis::SimViewer->_pause = !MengeVis::SimViewer->_pause;
			cout << "Timestamp: " << _lastTimestamp << endl;
			return true;
		}

		return false;
	}

}	// namespace Menge 
