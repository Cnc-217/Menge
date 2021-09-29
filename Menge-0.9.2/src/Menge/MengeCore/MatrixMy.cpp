#include "MatrixMy.h"
#include "Menge/MengeCore/Core.h"

namespace Menge {

	using namespace std;

	/////////////////////////////////////////////////////////////////////
	//					Implementation of MatrixFile
	/////////////////////////////////////////////////////////////////////

	int Matrix::getFileRows(const char* fileName) {
		ifstream fileStream;
		string tmp;
		int count = 0;// 行数计数器
		fileStream.open(fileName, ios::in);//ios::in 表示以只读的方式读取文件
		if (fileStream.fail())//文件打开失败:返回0
		{
			return 0;
		}
		else//文件存在
		{
			while (getline(fileStream, tmp, '\n'))//读取一行
			{
				if (tmp.size() > 0)
					count++;
			}
			fileStream.close();
			return count;
		}
	}

	int Matrix::getFileColumns(const char* fileName) {
		ifstream fileStream;
		fileStream.open(fileName, std::ios::_Nocreate);

		double tmp = 0;
		int count = 0;	// 列数计数器	
		char c;			//当前位置的字符
		c = fileStream.peek();
		while (('\n' != c) && (!fileStream.eof()))	// 指针指向的当前字符，仅观测，不移动指针位置
		{
			fileStream >> tmp;
			++count;
			c = fileStream.peek();
		}

		fileStream.close();
		return count;
	}

	void Matrix::getFileData(const char* fileName, int rowNum, int colNum)
	{
		MatrixDim2* tmpMatrix = new MatrixDim2(rowNum, colNum, 1);
		ifstream fileStream;
		string oneLine = "";	//输入文件的某一行
		double tmp = 0;		//当前位置上的数值
		int rowCount;	// 行数计数器
		int colCount = 0;	// 列数计数器
		int index = 0;		// 当前位置在X[]数组的下标
		int maxIndex = rowNum * (colNum - 1) - 1;

		// 打开文件
		fileStream.open(fileName, ios::in);	//ios::in 表示以只读的方式读取文件
		//文件打开失败:返回0
		if (fileStream.fail()){
			cout << "文件不存在." << endl;
			fileStream.close();
			system("pause");
			exit(1);
		}
		//文件存在
		else{
			// 读入数据
			rowCount = 0;	//初始化当前行数为0
			colCount = 0;	//当前列数清零
			float tmp = 0;	//当前读入的数值
			while (!fileStream.eof()) {
				fileStream >> tmp;

				if (colCount == 0) {	//第一列
					if (rowCount < rowNum) {		//越界检查
						tmpMatrix->SetPoint(rowCount, colCount, tmp);
					}
					else {
						cout << "计算出的行数与输入数据不符，请检查数据（如文件末尾不能有空行）." << endl;
						fileStream.close();
						system("pause");
						exit(1);
					}
				}
				else {				//非第一列
					index = rowCount * (colNum - 1) + colCount - 1;
					if (index <= maxIndex) {		//越界检查
						tmpMatrix->SetPoint(rowCount, colCount, tmp);
					}
					else {
						cout << "X[]下标超出数组范围，可能是文件中某行的列数>第一行的列数，退出！" << endl;
						fileStream.close();
						system("pause");
						exit(1);
					}
				}
				if ('\n' != fileStream.peek()) {	// 未到行尾，colCount累加，rowCount不变

					++colCount;

				}
				else {	//已到行尾，colCount清零，rowCount累加				
					if ((colCount + 1) != colNum)		//越界检查
					{
						cout << "第" << rowCount + 1 << "行，输入的列数与文件列数不一致，请检查数据." << endl;
						fileStream.close();
						system("pause");
						exit(1);
					}
					else {
						rowCount++;	// 换下一行
						colCount = 0;	// 列数清零				
					}
				}
			}

			Menge::BaseScene::ProbMatrix = tmpMatrix;
			// 关闭文件
			fileStream.close();
		}
	}

	/////////////////////////////////////////////////////////////////////
	//					Implementation of MatrixDim2
	/////////////////////////////////////////////////////////////////////
	void MatrixDim2::initialize() {//初始化矩阵大小
		p = new float* [rows_num];//分配rows_num个指针
		for (int i = 0; i < rows_num; ++i) {
			p[i] = new float[cols_num];//为p[i]进行动态内存分配，大小为cols
		}
	}

	//声明一个值全部为value的矩阵
	MatrixDim2::MatrixDim2(int rows, int cols, float value)
	{
		_sumWeight = new std::vector<float>();
		rows_num = rows;
		cols_num = cols;
		initialize();
		float sum = 0;
		for (int i = 0; i < rows_num; i++) {
			for (int j = 0; j < cols_num; j++) {
				p[i][j] = value;
				sum += p[i][j];
			}
			_sumWeight->push_back(sum);
			sum = 0;
		}

	}

	//析构函数
	MatrixDim2::~MatrixDim2() {
		for (int i = 0; i < rows_num; ++i) {
			delete[] p[i];
		}
		delete[] p;
	}

	//矩阵显示
	void MatrixDim2::Show() const {
		//cout << rows_num <<" "<<cols_num<< endl;//显示矩阵的行数和列数
		for (int i = 0; i < rows_num; i++) {
			for (int j = 0; j < cols_num; j++) {
				cout << p[i][j] << " ";
			}
			cout << endl;
		}
		cout << endl;
	}
	

	//返回矩阵第i行第j列的数
	float MatrixDim2::Point(int i, int j) const {
		return this->p[i][j];
	}

	//设置矩阵第i行第j列的值
	void MatrixDim2::SetPoint(int i, int j, float value) {
		p[i][j] = value;
	}

	//读取矩阵行列数
	int MatrixDim2::row_size() const {
		return rows_num;
	}
	int MatrixDim2::col_size() const {
		return cols_num;
	}

	//初始化_sumWeight
	void MatrixDim2::InitSumWeight() {
		_sumWeight->clear();
		float sum = 0;
		for (int i = 0; i < rows_num; i++) {
			for (int j = 0; j < cols_num; j++) {
				sum += p[i][j];
			}
			_sumWeight->push_back(sum);
			sum = 0;
		}
	}

	vector<vector<float>> MatrixDim2::toVector() {
		vector<vector<float>> res;
		for (int i = 0; i < rows_num; ++i) {
			vector<float> row;
			for (int j = 0; j < cols_num; ++j) {
				row.push_back(p[i][j]);
			}
			res.push_back(row);
			row.clear();
		}
		
		return res;
	}

	/////////////////////////////////////////////////////////////////////
	//					Implementation of MatrixDim3
	/////////////////////////////////////////////////////////////////////
	void MatrixDim3::initialize() {//初始化矩阵大小
		p = new float** [dimension_a];//分配dimension_a个指针
		for (int i = 0; i < dimension_a; ++i) {
			p[i] = new float* [dimension_b];
			for (int j = 0; j < dimension_b; ++j) {
				p[i][j] = new float[dimension_c];//为p[i][j]进行动态内存分配，大小为dimension_c
			}
		}
	}


	//声明一个值全部为value的矩阵
	MatrixDim3::MatrixDim3(int a, int b, int c, float value)
	{
		dimension_a = a;
		dimension_b = b;
		dimension_c = c;
		initialize();
		for (int i = 0; i < dimension_a; i++) {
			for (int j = 0; j < dimension_b; j++) {
				for (int k = 0; k < dimension_c; k++) {
					p[i][j][k] = value;
				}
			}
		}
	}

	//析构函数
	MatrixDim3::~MatrixDim3() {
		for (int i = 0; i < dimension_a; ++i) {
			for (int j = 0; j < dimension_b; j++) {
				delete[] p[i][j];
			}
			delete[] p[i];
		}
		delete[] p;
	}

	//矩阵显示
	void MatrixDim3::Show() const {
		//cout << dimension_a <<" "<< dimension_b <<" "<< dimension_c << endl;
		for (int i = 0; i < dimension_a; i++) {
			for (int j = 0; j < dimension_b; j++) {
				for (int k = 0; k < dimension_c; k++) {
					cout << p[i][j][k] << " ";
				}
			}
			cout << " " << endl;
		}
		cout <<" "<< endl;
	}

	//返回矩阵第i行第j列的数
	float MatrixDim3::Point(int i, int j, int k) const{
		//cout << "point: "<<p[i][j][k] << endl;
		return this->p[i][j][k];
	}

	//设置矩阵i,j,k的值
	void MatrixDim3::SetPoint(int i, int j,int k, float value) {
		p[i][j][k] = value;
	}

	//读取矩阵三维尺寸
	int MatrixDim3::dimensionA_size() const {
		return dimension_a;
	}
	int MatrixDim3::dimensionB_size() const {
		return dimension_b;
	}

	int MatrixDim3::dimensionC_size() const {
		return dimension_c;
	}

	//typeAgent:0 1 2   shopID:0~29   strength:0 1 2 3
	//3*3*10*1 第一维：顾客类型；第二维：当前state类型；第三维：店铺id0~9；值：选择店铺的概率
	void MatrixDim3::Aggregate(int typeAgent, int shopID, int strength) { 
		float temp = this->Point(typeAgent, shopID / 10, shopID % 10);
		float sum = 0;
		if (this->Point(typeAgent, shopID / 10, shopID % 10) > 8.5) return;
		for (int k = 0; k < this->dimensionC_size(); k++) { //shopID
			if (typeAgent == shopID / 10) {
				if (k != shopID % 10){
					float prob = this->Point(typeAgent, shopID / 10, k) ;
					sum += prob * 0.12 * strength;//0.12 0.24 0.36
					this->SetPoint(typeAgent, shopID / 10, k, prob * (1-0.12 * strength));
				}	
			}
			else {
				if (k != shopID % 10) {
					float prob = this->Point(typeAgent, shopID / 10, k);
					sum += prob * 0.08 * strength;//0.08 0.16 0.24
					this->SetPoint(typeAgent, shopID / 10, k, prob * (1 - 0.08 * strength));
				}
			}
		}
		this->SetPoint(typeAgent, shopID / 10, shopID % 10, temp+sum);
	}


	/////////////////////////////////////////////////////////////////////
	//					Implementation of MatrixDim4
	/////////////////////////////////////////////////////////////////////
	void MatrixDim4::initialize() {//初始化矩阵大小
		p = new float*** [dimension_a];//分配dimension_a个指针
		for (int i = 0; i < dimension_a; ++i) {
			p[i] = new float** [dimension_b];
			for (int j = 0; j < dimension_b; ++j) {
				p[i][j] = new float* [dimension_c];
				for (int k = 0; k < dimension_c; ++k) {
					p[i][j][k] = new float [dimension_d];//为p[i][j]进行动态内存分配，大小为dimension_d
				}
			}
		}
	}

	

	//声明一个值全部为value的矩阵
	MatrixDim4::MatrixDim4(int a, int b, int c, int d, float value) 
	{
		dimension_a = a;
		dimension_b = b;
		dimension_c = c;
		dimension_d = d;
		initialize();
		for (int i = 0; i < dimension_a; i++) {
			for (int j = 0; j < dimension_b; j++) {
				for (int k = 0; k < dimension_c; k++) {
					for (int l = 0; l < dimension_d; l++) {
						p[i][j][k][l] = value;
					}
				}
			}
		}
		_sumWeightMatrix = new MatrixDim2(3,30,0);
		
		
		for (int i = 0; i < dimension_a; i++) {
			for (int j = 0; j < dimension_b; j++) {
				for (int k = 0; k < dimension_c; k++) {
					for (int l = 0; l < dimension_d; l++) {
						_sumWeightMatrix->SetPoint(i, j, _sumWeightMatrix->Point(i, j)+p[i][j][k][l]);
					}			
				}
			}
		}

	}


	//析构函数
	MatrixDim4::~MatrixDim4() {
		for (int i = 0; i < dimension_a; ++i) {
			for (int j = 0; j < dimension_b; j++) {
				delete[] p[i][j];
				for (int k = 0; k < dimension_c; k++) {
					delete[] p[i][j][k];
				}
			}
			delete[] p[i];
		}
		delete[] p;
	}

	//矩阵显示
	void MatrixDim4::Show() const {
		//cout << rows_num <<" "<<cols_num<< endl;//显示矩阵的行数和列数
		for (int i = 0; i < dimension_a; i++) {
			for (int j = 0; j < dimension_b; j++) {
				for (int k = 0; k < dimension_c; k++) {
					for (int l = 0; l < dimension_d; l++) {
						cout << p[i][j][k][l] << " ";
					}
				}
				cout << endl;
			}
			cout << endl;
		}
		cout << endl;
	}

	//返回矩阵i j k l的数
	float MatrixDim4::Point(int i, int j, int k,int l ) const {
		return this->p[i][j][k][l];
	}

	//设置矩阵i,j,k,l的值
	void MatrixDim4::SetPoint(int i, int j, int k, int l ,float value){
		p[i][j][k][l] = value;
	}

	//读取矩阵尺寸
	void MatrixDim4::dimension_size() {
		cout << "dimension_a: " << this->dimension_a << endl;
		cout << "dimension_b: " << this->dimension_b << endl;
		cout << "dimension_c: " << this->dimension_c << endl;
		cout << "dimension_d: " << this->dimension_d << endl;
	}
	

	//聚集策略
	void MatrixDim4::Aggregate(int typeAgent, int shopID, int strength){
		//发优惠券只会影响相同类型的店铺，不会影响不同类型的店铺
		
		for (int i = 0; i < dimension_b; i++) {  //对于每一家起点店铺
			if (this->Point(typeAgent, i,shopID / 10, shopID % 10) > 8.5) return;
			
			float temp = this->Point(typeAgent, i, shopID / 10, shopID % 10);//记录选中的那家的权重
			float sum = 0;

			for (int j = 0; j < dimension_d; j++) {//改变同类型店铺的概率
				if (typeAgent == shopID / 10) {     //给agent发放他倾向的优惠券

					float prob = this->Point(typeAgent, i,shopID / 10, j);
					sum += prob * 0.12 * strength;//0.12 0.24 0.36
					this->SetPoint(typeAgent, i,shopID / 10, j, prob * (1 - 0.12 * strength));

				}
					
				else {  //发放不倾向的优惠券
					float prob = this->Point(typeAgent, i, shopID / 10, j);
					sum += prob * 0.08 * strength;//0.12 0.24 0.36
					this->SetPoint(typeAgent, i, shopID / 10, j, prob * (1 - 0.08 * strength));
				}
			}

			this->SetPoint(typeAgent, i,shopID / 10, shopID % 10, temp + sum);

		}
		
	}

}