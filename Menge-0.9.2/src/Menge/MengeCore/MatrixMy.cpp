#include "MatrixMy.h"

namespace Menge {

	using namespace std;

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
		rows_num = rows;
		cols_num = cols;
		initialize();
		for (int i = 0; i < rows_num; i++) {
			for (int j = 0; j < cols_num; j++) {
				p[i][j] = value;
			}
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
		//_sumWeightMatrix = matrix;
		
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