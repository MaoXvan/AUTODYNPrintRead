//一个cycle里含有若干个section，一个section负责打印若干个变量，只有第一个section打印SPH粒子的材料信息
//一个section分成若干个segment，每个segment有一个信息头header, header里只有三个非空行，依次为:
//(1)"CYCLE.*,.*TIME=.*IMAX=.*JMAX=.*KMAX=.*"
//(2)"I=.*,K=.*"(根据SPHGridDataInfo里order的不同会有区别，比如可能是"I=.*,J=.*")
//(3)数据变量名行
//**************************************************
//文件内出现的除换行符以外的空白符一般都是char(0x20)即space,不会出现水平制表符(0x09)和垂直制表符等
//所以默认所有非换行空白符都是space
//prt文件头具有模拟所需要的所有信息，本程序只提取部分XFCT用得到的重要信息，其他信息暂时不用
//1.prt文件内的网格变量名若由多个单词一起构成，则相邻两个单词之间空格数量小于等于1
//2.prt文件内的材料名若由多个单词一起构成，则相邻两个单词之间空格数量小于等于1
//3.默认不同的网格变量名之间空格数量大于等于2
//4.默认材料名最多由空格间隔至多为1的两个单词组成
//5.如果材料名后面出现网格变量的数值，则材料名与数值之间的空格至少为1
//6.默认每段数据的开头与上一段数据的结尾之间的过渡部分具有下面的固定格式：
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
//每段数据的数据头具有"CYCLE    1046, TIME = 3.302E-03                SUBGRID: OFHC - Al(SPH), IMAX = 1, JMAX = 913104, KMAX = 1"的格式
//(1)每段数据的数据头与前段数据最后一行数据有至少一行空行
//(2)每段数据的数据头与节点索引行"I = 1, K = 1"至少相隔一行空行
//(3)PART索引行"I = 1, K = 1"与数据名称行:
//"J  MATERIAL    MAT.FRAC    MASS        INT.ENERGY  COMPRESS    TEMP        X           Y           Z           X - VELOCITY"
//至少相隔一行空行
//
//(4)数据名称行与数据行首行没有空行
//(5)数据行中相邻两个数值之间一般间隔两个空格，但若后一个数值是具有正负号那么只会间隔一个空格
//(6)数据行中材料名前面一定是节点索引后面一定是某个网格变量数值，都间隔至少一个空格
//
//因为AUTODYN prt文件的列数数量有限制，当要显示的变量数值总长度超出列数限制时，AUTODYN会将数据总体分成多个部分进行显示，比如上述示例中一共输出16个网格变量
//超出列数限制，被分成两部分分别输出(当需要打印更多变量时会分成更多部分). 假设有三个部分，各部分按打印先后次序记为B0，B1和B2部分，首先打印输出B0部分
//然后打印输出B1部分，最后输出B2部分
//(1)B0，B1和B2各有一套节点索引，且都从1开始直到最后一个，比如上述示例中从J = 1到J = JMAX = 913104
//(2)因为具有相同节点索引的节点具有相同的材料，所以只有B0部分的数据段有各节点材料名，B1和B2部分不再有材料名,只能根据索引得知材料名
//(3)B0和B1之间以及B1和B2之间的过度与数据段之间的过度格式相同，下面展示了B0和B1之间数据段过度例子:
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
//当prt文件内具有不同时间步的数据时，不同时间步按先后次序打印，可通过每段数据的数据头
//"CYCLE    1046, TIME= 3.302E-03                SUBGRID: OFHC-Al    (SPH          ), IMAX=  1, JMAX= 913104, KMAX=  1"
//判断后文数据属于哪一步
//一般出现CYCLE时代表后文是数据段，但有时会出现如下情况:
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
//"CYCLE    1028, TIME=  3.245E-03"这一行后面并不是完整的数据段，这种情形一般只会出现在每个时间步数据的开头，在同一个时间步数据内不会出现
//出现这种情况的原因暂不明确
//为了规避这种情况，最好判断该行后面是否紧接着形如"I =   1, K =   1"的节点索引行，如果出现则表明后面是一段有效的数据段
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