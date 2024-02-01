//һ��cycle�ﺬ�����ɸ�section��һ��section�����ӡ���ɸ�������ֻ�е�һ��section��ӡSPH���ӵĲ�����Ϣ
//һ��section�ֳ����ɸ�segment��ÿ��segment��һ����Ϣͷheader, header��ֻ�������ǿ��У�����Ϊ:
//(1)"CYCLE.*,.*TIME=.*IMAX=.*JMAX=.*KMAX=.*"
//(2)"I=.*,K=.*"(����SPHGridDataInfo��order�Ĳ�ͬ�������𣬱��������"I=.*,J=.*")
//(3)���ݱ�������
//**************************************************
//�ļ��ڳ��ֵĳ����з�����Ŀհ׷�һ�㶼��char(0x20)��space,�������ˮƽ�Ʊ��(0x09)�ʹ�ֱ�Ʊ����
//����Ĭ�����зǻ��пհ׷�����space
//prt�ļ�ͷ����ģ������Ҫ��������Ϣ��������ֻ��ȡ����XFCT�õõ�����Ҫ��Ϣ��������Ϣ��ʱ����
//1.prt�ļ��ڵ�������������ɶ������һ�𹹳ɣ���������������֮��ո�����С�ڵ���1
//2.prt�ļ��ڵĲ��������ɶ������һ�𹹳ɣ���������������֮��ո�����С�ڵ���1
//3.Ĭ�ϲ�ͬ�����������֮��ո��������ڵ���2
//4.Ĭ�ϲ���������ɿո�������Ϊ1�������������
//5.���������������������������ֵ�������������ֵ֮��Ŀո�����Ϊ1
//6.Ĭ��ÿ�����ݵĿ�ͷ����һ�����ݵĽ�β֮��Ĺ��ɲ��־�������Ĺ̶���ʽ��
//---------------------------------------------------------------------------------------------------------------------------------
//	15911  CU-OFHC     0.0000E+00  8.9300E-03  1.6169E+05 -5.7197E-01  2.0437E+02  1.6321E+00  2.3970E+00  1.5480E+01  2.9381E+02
//	15912  CU-OFHC     0.0000E+00  8.9300E-03  1.5762E+05 -5.9351E-01  2.0154E+02  1.8312E+00  2.4237E+00  1.5462E+01  3.3102E+02
//	15913  CU-OFHC     0.0000E+00  8.9300E-03  1.5274E+05 -6.2316E-01  1.9783E+02  2.0363E+00  2.4352E+00  1.5500E+01  3.7075E+02
//
//	CYCLE    1046, TIME = 3.302E-03                SUBGRID: OFHC - Al(SPH), IMAX = 1, JMAX = 913104, KMAX = 1
//
//
//
//	I = 1, K = 1
//
//	J  MATERIAL    MAT.FRAC    MASS        INT.ENERGY  COMPRESS    TEMP        X           Y           Z           X - VELOCITY
//	15914  CU-OFHC     0.0000E+00  8.9300E-03  1.4717E+05 -6.1349E-01  1.9374E+02  2.2435E+00  2.4428E+00  1.5474E+01  4.1111E+02
//	15915  CU-OFHC     0.0000E+00  8.9300E-03  1.4189E+05 -6.2841E-01  1.8949E+02  2.4545E+00  2.4583E+00  1.5511E+01  4.5284E+02
//	15916  CU-OFHC     0.0000E+00  8.9300E-03  1.3563E+05 -6.1105E-01  1.8460E+02  2.6621E+00  2.4650E+00  1.5503E+01  4.9308E+02
//---------------------------------------------------------------------------------------------------------------------------------
//ÿ�����ݵ�����ͷ����"CYCLE    1046, TIME = 3.302E-03                SUBGRID: OFHC - Al(SPH), IMAX = 1, JMAX = 913104, KMAX = 1"�ĸ�ʽ
//(1)ÿ�����ݵ�����ͷ��ǰ���������һ������������һ�п���
//(2)ÿ�����ݵ�����ͷ��ڵ�������"I = 1, K = 1"�������һ�п���
//(3)PART������"I = 1, K = 1"������������:
//"J  MATERIAL    MAT.FRAC    MASS        INT.ENERGY  COMPRESS    TEMP        X           Y           Z           X - VELOCITY"
//�������һ�п���
//
//(4)����������������������û�п���
//(5)������������������ֵ֮��һ���������ո񣬵�����һ����ֵ�Ǿ�����������ôֻ����һ���ո�
//(6)�������в�����ǰ��һ���ǽڵ���������һ����ĳ�����������ֵ�����������һ���ո�
//
//��ΪAUTODYN prt�ļ����������������ƣ���Ҫ��ʾ�ı�����ֵ�ܳ��ȳ�����������ʱ��AUTODYN�Ὣ��������ֳɶ�����ֽ�����ʾ����������ʾ����һ�����16���������
//�����������ƣ����ֳ������ֱַ����(����Ҫ��ӡ�������ʱ��ֳɸ��ಿ��). �������������֣������ְ���ӡ�Ⱥ�����ΪB0��B1��B2���֣����ȴ�ӡ���B0����
//Ȼ���ӡ���B1���֣�������B2����
//(1)B0��B1��B2����һ�׽ڵ��������Ҷ���1��ʼֱ�����һ������������ʾ���д�J = 1��J = JMAX = 913104
//(2)��Ϊ������ͬ�ڵ������Ľڵ������ͬ�Ĳ��ϣ�����ֻ��B0���ֵ����ݶ��и��ڵ��������B1��B2���ֲ����в�����,ֻ�ܸ���������֪������
//(3)B0��B1֮���Լ�B1��B2֮��Ĺ��������ݶ�֮��Ĺ��ȸ�ʽ��ͬ������չʾ��B0��B1֮�����ݶι�������:
/*
-----------------------------------------------------------------------------------------------------------------------------------
 913100  AL2024T351  0.0000E+00  2.7850E-03  1.0886E+02  4.5065E-04  3.0036E+02  9.5841E+00  9.9840E+00  9.5121E-01  2.7313E+01
 913101  AL2024T351  0.0000E+00  2.7850E-03  1.0731E+02  3.5513E-04  3.0030E+02  9.6837E+00  9.9840E+00  9.5152E-01  2.6352E+01
 913102  AL2024T351  0.0000E+00  2.7850E-03  1.3451E+02  2.4369E-04  3.0026E+02  9.7836E+00  9.9839E+00  9.5215E-01  2.8983E+01
 913103  AL2024T351  0.0000E+00  2.7850E-03  1.3329E+02  9.0618E-05  3.0014E+02  9.8833E+00  9.9837E+00  9.5290E-01  2.8704E+01
 913104  AL2024T351  0.0000E+00  2.7850E-03  1.1331E+02 -4.6157E-04  2.9977E+02  9.9834E+00  9.9835E+00  9.5400E-01  2.7882E+01

CYCLE    1046, TIME= 3.302E-03                SUBGRID: OFHC-Al    (SPH          ), IMAX=  1, JMAX= 913104, KMAX=  1



I =   1, K =   1

J  Y-VELOCITY  Z-VELOCITY  NODE MASS   DENSITY     SMOOTH.LEN  DEN.OLD     NO.NEIGH
1  1.5005E+01  5.9682E+03  8.9300E-03  4.3541E+00  1.2705E-01  4.3553E+00  5.9000E+01
2  8.2437E+00  5.9864E+03  8.9300E-03  4.2948E+00  1.2763E-01  4.2962E+00  5.7000E+01
3  2.3699E+01  5.9543E+03  8.9300E-03  4.5053E+00  1.2561E-01  4.5059E+00  6.0000E+01
---------------------------------------------------------------------------------------------------------------------------------------
*/
//��prt�ļ��ھ��в�ͬʱ�䲽������ʱ����ͬʱ�䲽���Ⱥ�����ӡ����ͨ��ÿ�����ݵ�����ͷ
//"CYCLE    1046, TIME= 3.302E-03                SUBGRID: OFHC-Al    (SPH          ), IMAX=  1, JMAX= 913104, KMAX=  1"
//�жϺ�������������һ��
//һ�����CYCLEʱ������������ݶΣ�����ʱ������������:
/*
-------------------------------------------------------------------------------------------------------------------------------------
CYCLE    1028, TIME=  3.245E-03                                                                                  CYCLE SUMMARY EDITS


 CYCLE         TIME           DT       DTSTAB  @   I   J   K SUB   SOUNDSPEED    DIMENSION
	1028   3.2452E-03   3.1589E-06   3.1589E-06      1****   1   1   0.0000E+00   1.0774E-01

CYCLE    1046, TIME= 3.302E-03                SUBGRID: OFHC-Al    (SPH          ), IMAX=  1, JMAX= 913104, KMAX=  1



I =   1, K =   1

	  J  MATERIAL    MAT.FRAC    MASS        INT.ENERGY  COMPRESS    TEMP        X           Y           Z           X-VELOCITY
	  1  CU-OFHC     0.0000E+00  8.9300E-03  2.2288E+05 -5.1241E-01  2.4998E+02  1.0579E-01  9.1317E-02  1.5565E+01  2.1134E+01
	  2  CU-OFHC     0.0000E+00  8.9300E-03  2.2147E+05 -5.1906E-01  2.4949E+02  2.8923E-01  7.4283E-02  1.5613E+01  5.1850E+01
	  3  CU-OFHC     0.0000E+00  8.9300E-03  2.2123E+05 -4.9549E-01  2.4957E+02  4.8341E-01  1.1272E-01  1.5528E+01  8.6931E+01
----------------------------------------------------------------------------------------------------------------------------------
//"CYCLE    1028, TIME=  3.245E-03"��һ�к��沢�������������ݶΣ���������һ��ֻ�������ÿ��ʱ�䲽���ݵĿ�ͷ����ͬһ��ʱ�䲽�����ڲ������
//�������������ԭ���ݲ���ȷ
//Ϊ�˹���������������жϸ��к����Ƿ����������"I =   1, K =   1"�Ľڵ������У�������������������һ����Ч�����ݶ�
*/
//**************************************************

#pragma once
#include<algorithm>
#include<ctype.h>
#include<exception>
#include<fstream>
#include<iostream>
#include<numeric>
#include<regex>
#include<stdio.h>
#include<string>
#include<sstream>
#include<vector>

//the amount of SPH particles is about 1E6.
const size_t kSPHParticles = 1000000;
const size_t kSprintfBufferSize = 256;

class ErrorMessage
{
public:
	ErrorMessage(bool success = true, int id = 0, std::string e = "");
	//ErrorMessage(const ErrorMessage& error_message);
	~ErrorMessage() {}
	void SetErrorInfo(bool success, int id, std::string e);
	bool is_successful;
	int error_id;
	std::string error;
};

struct DATALINE
{
	int index;
	std::string material;
	std::vector<double> data;
};

struct SPHGridDataInfo
{
	std::vector<std::string> units;
	std::vector<std::string> material_names;
	std::vector<double> reference_density;
	std::vector<std::string> density_unit;
	std::vector<std::string> object_names;
	std::vector<int> object_particles;

	int I_BEGIN;
	int I_END;
	int J_BEGIN;
	int J_END;
	int K_BEGIN;
	int K_END;
	int I_MAX;
	int J_MAX;
	int K_MAX;
	std::string order;

	int total_number_nodes;
	std::streampos first_cycle_start;
};

class SPHGridData
{
public:
	SPHGridData();
	~SPHGridData();
	
	bool SeriesTo2DMatrix(std::vector<std::vector<double>>& matrix);
	ErrorMessage ReadFirstCycle(std::ifstream& ptr_autodyn, const SPHGridDataInfo& file_header);
	ErrorMessage ReadNextCycle(std::ifstream& prt_autodyn);
	void ResetSPHGridData();
	int cycle_id;
	double cycle_time;
	int sph_particles_amount;
	bool is_last_cycle;
	std::vector<int> scientific_notation_position;//save the first scientific notation position of every section in a cycle
	std::vector<double> series_griddata;
	std::vector<int> variable_amount;//save the amount of variables of every section in a cycle
	std::vector<int> variable_amount_first_dataline;//save the amount of variables of every section in a cycle
	std::vector<std::string> variable_name;//save variable names
	std::vector<std::string> material;//save material name of every SPH particle

private:
	ErrorMessage AppendSeriesData(const std::string& line, int pos);
};



int GetFirstRegexMatchPosition(const std::string& line, const std::regex& pattern);
int GetFirstNonblankCharacterPosition(const std::string& s);
std::string GetMaterialFromDataLine(const std::string& line, int particle_index_position, int first_scientific_notation_position);
std::string GetMaterialFromDataLine(const std::string& line);
int GetIndexFromDataLine(const std::string& line);
int GetCycleIDFromSegmentTitle(const std::string& title);
double GetTimeFromSegmentTitle(const std::string& title);
std::string GetFirstNonBlankSubString(const std::string& line);
bool ReadNonblankLines(std::ifstream& prt_autodyn, std::vector<std::string>& res, int k);
bool ReadCycleTimeFromDataLine(const std::string& line, int& cycle_id, double& cycle_time);
std::string ReadFileHeader(std::ifstream& ptr_in_fstream, SPHGridDataInfo& file_header);
std::string SlimString(const std::string& line);
std::vector<std::string> SplitString(const std::string& line, std::string separator);
bool SplitString(const std::string& line, int blanks, std::vector<std::string>& res);

int GetNumberOfWords(const std::string& line, int pos);