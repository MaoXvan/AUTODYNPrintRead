

#include "AutodynSPHPrintReader.h"
using std::cout;
using std::endl;
using std::regex;
using std::string;
using std::streampos;
using std::istringstream;
using std::vector;
using std::ifstream;
using std::getline;//读取一行且不含换行符


int main(int argc,char* argv[])
{
	string error_message;
	argc = 2;
	if (argc == 1)
	{
		printf("File name is empty!\n");
		return 0;
	}
	if (argc > 2)
	{
		printf("Too much input!\n");
		return 0;
	}
	
	const char* filename = argv[1];
	filename = R"(D:\ZZZ\AutoDyn\FXR\autodyn.prt)";
	ifstream prt_autodyn;
	
	prt_autodyn.open(filename, std::ifstream::in);
	if (!prt_autodyn.is_open())
	{
		error_message = string("Fail to open file:") + string(filename)+string("\n");
		printf(error_message.c_str());
		return 0;
	}
	
	SPHGridDataInfo file_header;
	vector<int> Cycle;
	vector<double> Time;
	SPHGridData sph_data;
	string line = ReadFileHeader(prt_autodyn, file_header);//这里的line是第一个cycle数据段头的第一行,即"CYCLE.*,.*TIME=.*IMAX=.*JMAX=.*KMAX=.*"这一行
	//但此时的文件指针在这一行的后一行的开始位置
	//file_header.data_section_start保存第一个"CYCLE.*,.*TIME=.*IMAX=.*JMAX=.*KMAX=.*"开始位置处的指针
	prt_autodyn.seekg(file_header.first_cycle_start, std::ios::beg);//文件指针重定位到第一个cycle行开始处
	ErrorMessage message = sph_data.ReadFirstCycle(prt_autodyn, file_header);
	cout << "Cycle = " << sph_data.cycle_id << " , Time = " << sph_data.cycle_time << endl;
	while (!sph_data.is_last_cycle && message.is_successful)
	{
		message = sph_data.ReadNextCycle(prt_autodyn);
		cout << "Cycle = " << sph_data.cycle_id << " , Time = " << sph_data.cycle_time << endl;
	}
	prt_autodyn.close();
	return 0;
}
//






