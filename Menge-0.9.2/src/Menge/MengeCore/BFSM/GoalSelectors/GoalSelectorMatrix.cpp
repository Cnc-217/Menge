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

#include "MengeCore/BFSM/GoalSelectors/GoalSelectorMatrix.h"

#include "MengeCore/BFSM/GoalSet.h"
#include "MengeCore/BFSM/Goals/Goal.h"
#include "MengeCore/Core.h"
#include <cstdlib> // Header file needed to use srand and rand
#include <ctime> // Header file needed to use time
#include <cassert>
using namespace Menge::Olympic;
namespace Menge {

	namespace BFSM {

		MatrixGoalSelector::MatrixGoalSelector() {
			Menge::Olympic::goalSeclectorType = "Matrix";
		}

		/////////////////////////////////////////////////////////////////////
		//                   Implementation of WeightedGoalSelector
		/////////////////////////////////////////////////////////////////////
		
		Goal * MatrixGoalSelector::getGoal( const Agents::BaseAgent * agent ) const {
			//if (Menge::SIM_TIME > 0)
				return _goalSet->getGoalFromMatrix(agent);
				/*
			{
				//return _goalSet->getGoalFromMatrix(agent);
				
				assert(agent != 0x0 && "MatrixGoalSelector requires a valid base agent!");
				srand(time(0));
				int lastShopGone = agent->_shopGone.back();
				//int lastShopGone2 = agent->_shopGone2.back();
				int travelNum = agent->_shopGoneNum;//最近走过的五个店中同类店铺数量

				float randomNumber = rand() % 100 * 0.01 ;//取概率
				if (travelNum < shopInfo[lastShopGone].sameSetGoalNum)//如果不够最大数量
				{	
					if (randomNumber < 0.9)//在一个大概率下
					{
						if (agent->_id == 1)
							cout << "-----------SameRegion------------" << endl;
						return _goalSet->getGoalSameRegion(agent);//仍在本区域选择
					}
					else//有0.1的可能跳出去选择原算法
					{
						if (agent->_id == 1)
							cout << "-----------origin------------" << endl;
						return _goalSet->getGoalFromMatrix(agent);//原来的
					}
				}
				else//如果已经走了最大的数量
				{
					if (agent->_id == 1)
						cout << "-----------origin------------" << endl;
					return _goalSet->getGoalFromMatrix(agent);//就跳出去
				}
				
			}*/
			//else
				//return _goalSet->getRandomGoal();//第一次选择
				//return _goalSet->getGoalByID(34);

		}
	}	// namespace BFSM
}	// namespace Menge
