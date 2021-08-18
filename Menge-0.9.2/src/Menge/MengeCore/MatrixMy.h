#pragma once
#ifndef __MATRIX_MY_H__
#define __MATRIX_MY_H__

#include <iostream>
#include <stdlib.h>
#include <vector>

namespace Menge{
	
	class MatrixDim2 {
	private:
		int rows_num, cols_num;
		float** p;
		void initialize();//初始化矩阵

	public:
		std::vector<float>* _sumWeight;
		MatrixDim2(int, int, float);//二维初始化，矩阵全为某个值
		virtual ~MatrixDim2();//析构函数应当是虚函数，除非此类不用做基类
		void Show() const;//矩阵显示2维
		float Point(int i, int j) const;//显示点的值
		void SetPoint(int i, int j, float value) ;//设置点的值
		int row_size() const;//返回行数
		int col_size() const;//返回列数
		void InitSumWeight(); //初始化_sumWeight

		void Aggregate(int typeAgent, int shopID, int strength);
	};


	class MatrixDim3 {
	private:
		int dimension_a, dimension_b, dimension_c;
		float*** p;
		void initialize();//初始化矩阵
	public:
		MatrixDim3(int, int, int, float);//三维初始化，矩阵全为某个值
		virtual ~MatrixDim3();//析构函数应当是虚函数，除非此类不用做基类
		void Show() const;//矩阵显示3维
		float Point(int i, int j, int k) const;
		void SetPoint(int i, int j, int k, float value) ;
		int dimensionA_size() const;//返回第一维尺寸
		int dimensionB_size() const;//返回第二维尺寸
		int dimensionC_size() const;//返回第三维尺寸

		void Aggregate(int typeAgent, int shopID, int strength) ;
	};

	
	class MatrixDim4 {
	private:
		int dimension_a, dimension_b, dimension_c, dimension_d;
		float**** p;
		void initialize();//初始化矩阵
		
	public:
		MatrixDim2* _sumWeightMatrix;
		MatrixDim4(int, int, int, int, float) ;//四维初始化，矩阵全为某个值
		virtual ~MatrixDim4();//析构函数应当是虚函数，除非此类不用做基类
		void Show() const;//矩阵显示
		float Point(int i, int j, int k, int l) const;
		void SetPoint(int i, int j, int k, int l,float value) ;
		void dimension_size();//返回尺寸


		void Aggregate(int typeAgent, int shopID, int strength) ;

	};
}
#endif