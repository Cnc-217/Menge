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
		int count = 0;// ����������
		fileStream.open(fileName, ios::in);//ios::in ��ʾ��ֻ���ķ�ʽ��ȡ�ļ�
		if (fileStream.fail())//�ļ���ʧ��:����0
		{
			return 0;
		}
		else//�ļ�����
		{
			while (getline(fileStream, tmp, '\n'))//��ȡһ��
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
		int count = 0;	// ����������	
		char c;			//��ǰλ�õ��ַ�
		c = fileStream.peek();
		while (('\n' != c) && (!fileStream.eof()))	// ָ��ָ��ĵ�ǰ�ַ������۲⣬���ƶ�ָ��λ��
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
		string oneLine = "";	//�����ļ���ĳһ��
		double tmp = 0;		//��ǰλ���ϵ���ֵ
		int rowCount;	// ����������
		int colCount = 0;	// ����������
		int index = 0;		// ��ǰλ����X[]������±�
		int maxIndex = rowNum * (colNum - 1) - 1;

		// ���ļ�
		fileStream.open(fileName, ios::in);	//ios::in ��ʾ��ֻ���ķ�ʽ��ȡ�ļ�
		//�ļ���ʧ��:����0
		if (fileStream.fail()){
			cout << "�ļ�������." << endl;
			fileStream.close();
			system("pause");
			exit(1);
		}
		//�ļ�����
		else{
			// ��������
			rowCount = 0;	//��ʼ����ǰ����Ϊ0
			colCount = 0;	//��ǰ��������
			float tmp = 0;	//��ǰ�������ֵ
			while (!fileStream.eof()) {
				fileStream >> tmp;

				if (colCount == 0) {	//��һ��
					if (rowCount < rowNum) {		//Խ����
						tmpMatrix->SetPoint(rowCount, colCount, tmp);
					}
					else {
						cout << "��������������������ݲ������������ݣ����ļ�ĩβ�����п��У�." << endl;
						fileStream.close();
						system("pause");
						exit(1);
					}
				}
				else {				//�ǵ�һ��
					index = rowCount * (colNum - 1) + colCount - 1;
					if (index <= maxIndex) {		//Խ����
						tmpMatrix->SetPoint(rowCount, colCount, tmp);
					}
					else {
						cout << "X[]�±곬�����鷶Χ���������ļ���ĳ�е�����>��һ�е��������˳���" << endl;
						fileStream.close();
						system("pause");
						exit(1);
					}
				}
				if ('\n' != fileStream.peek()) {	// δ����β��colCount�ۼӣ�rowCount����

					++colCount;

				}
				else {	//�ѵ���β��colCount���㣬rowCount�ۼ�				
					if ((colCount + 1) != colNum)		//Խ����
					{
						cout << "��" << rowCount + 1 << "�У�������������ļ�������һ�£���������." << endl;
						fileStream.close();
						system("pause");
						exit(1);
					}
					else {
						rowCount++;	// ����һ��
						colCount = 0;	// ��������				
					}
				}
			}

			Menge::BaseScene::ProbMatrix = tmpMatrix;
			// �ر��ļ�
			fileStream.close();
		}
	}

	/////////////////////////////////////////////////////////////////////
	//					Implementation of MatrixDim2
	/////////////////////////////////////////////////////////////////////
	void MatrixDim2::initialize() {//��ʼ�������С
		p = new float* [rows_num];//����rows_num��ָ��
		for (int i = 0; i < rows_num; ++i) {
			p[i] = new float[cols_num];//Ϊp[i]���ж�̬�ڴ���䣬��СΪcols
		}
	}

	//����һ��ֵȫ��Ϊvalue�ľ���
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

	//��������
	MatrixDim2::~MatrixDim2() {
		for (int i = 0; i < rows_num; ++i) {
			delete[] p[i];
		}
		delete[] p;
	}

	//������ʾ
	void MatrixDim2::Show() const {
		//cout << rows_num <<" "<<cols_num<< endl;//��ʾ���������������
		for (int i = 0; i < rows_num; i++) {
			for (int j = 0; j < cols_num; j++) {
				cout << p[i][j] << " ";
			}
			cout << endl;
		}
		cout << endl;
	}
	

	//���ؾ����i�е�j�е���
	float MatrixDim2::Point(int i, int j) const {
		return this->p[i][j];
	}

	//���þ����i�е�j�е�ֵ
	void MatrixDim2::SetPoint(int i, int j, float value) {
		p[i][j] = value;
	}

	//��ȡ����������
	int MatrixDim2::row_size() const {
		return rows_num;
	}
	int MatrixDim2::col_size() const {
		return cols_num;
	}

	//��ʼ��_sumWeight
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
	void MatrixDim3::initialize() {//��ʼ�������С
		p = new float** [dimension_a];//����dimension_a��ָ��
		for (int i = 0; i < dimension_a; ++i) {
			p[i] = new float* [dimension_b];
			for (int j = 0; j < dimension_b; ++j) {
				p[i][j] = new float[dimension_c];//Ϊp[i][j]���ж�̬�ڴ���䣬��СΪdimension_c
			}
		}
	}


	//����һ��ֵȫ��Ϊvalue�ľ���
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

	//��������
	MatrixDim3::~MatrixDim3() {
		for (int i = 0; i < dimension_a; ++i) {
			for (int j = 0; j < dimension_b; j++) {
				delete[] p[i][j];
			}
			delete[] p[i];
		}
		delete[] p;
	}

	//������ʾ
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

	//���ؾ����i�е�j�е���
	float MatrixDim3::Point(int i, int j, int k) const{
		//cout << "point: "<<p[i][j][k] << endl;
		return this->p[i][j][k];
	}

	//���þ���i,j,k��ֵ
	void MatrixDim3::SetPoint(int i, int j,int k, float value) {
		p[i][j][k] = value;
	}

	//��ȡ������ά�ߴ�
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
	//3*3*10*1 ��һά���˿����ͣ��ڶ�ά����ǰstate���ͣ�����ά������id0~9��ֵ��ѡ����̵ĸ���
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
	void MatrixDim4::initialize() {//��ʼ�������С
		p = new float*** [dimension_a];//����dimension_a��ָ��
		for (int i = 0; i < dimension_a; ++i) {
			p[i] = new float** [dimension_b];
			for (int j = 0; j < dimension_b; ++j) {
				p[i][j] = new float* [dimension_c];
				for (int k = 0; k < dimension_c; ++k) {
					p[i][j][k] = new float [dimension_d];//Ϊp[i][j]���ж�̬�ڴ���䣬��СΪdimension_d
				}
			}
		}
	}

	

	//����һ��ֵȫ��Ϊvalue�ľ���
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


	//��������
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

	//������ʾ
	void MatrixDim4::Show() const {
		//cout << rows_num <<" "<<cols_num<< endl;//��ʾ���������������
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

	//���ؾ���i j k l����
	float MatrixDim4::Point(int i, int j, int k,int l ) const {
		return this->p[i][j][k][l];
	}

	//���þ���i,j,k,l��ֵ
	void MatrixDim4::SetPoint(int i, int j, int k, int l ,float value){
		p[i][j][k][l] = value;
	}

	//��ȡ����ߴ�
	void MatrixDim4::dimension_size() {
		cout << "dimension_a: " << this->dimension_a << endl;
		cout << "dimension_b: " << this->dimension_b << endl;
		cout << "dimension_c: " << this->dimension_c << endl;
		cout << "dimension_d: " << this->dimension_d << endl;
	}
	

	//�ۼ�����
	void MatrixDim4::Aggregate(int typeAgent, int shopID, int strength){
		//���Ż�ȯֻ��Ӱ����ͬ���͵ĵ��̣�����Ӱ�첻ͬ���͵ĵ���
		
		for (int i = 0; i < dimension_b; i++) {  //����ÿһ��������
			if (this->Point(typeAgent, i,shopID / 10, shopID % 10) > 8.5) return;
			
			float temp = this->Point(typeAgent, i, shopID / 10, shopID % 10);//��¼ѡ�е��Ǽҵ�Ȩ��
			float sum = 0;

			for (int j = 0; j < dimension_d; j++) {//�ı�ͬ���͵��̵ĸ���
				if (typeAgent == shopID / 10) {     //��agent������������Ż�ȯ

					float prob = this->Point(typeAgent, i,shopID / 10, j);
					sum += prob * 0.12 * strength;//0.12 0.24 0.36
					this->SetPoint(typeAgent, i,shopID / 10, j, prob * (1 - 0.12 * strength));

				}
					
				else {  //���Ų�������Ż�ȯ
					float prob = this->Point(typeAgent, i, shopID / 10, j);
					sum += prob * 0.08 * strength;//0.12 0.24 0.36
					this->SetPoint(typeAgent, i, shopID / 10, j, prob * (1 - 0.08 * strength));
				}
			}

			this->SetPoint(typeAgent, i,shopID / 10, shopID % 10, temp + sum);

		}
		
	}

}