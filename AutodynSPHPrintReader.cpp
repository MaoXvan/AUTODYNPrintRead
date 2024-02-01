#include "AutodynSPHPrintReader.h"
using namespace std;
ErrorMessage::ErrorMessage(bool success, int id, std::string e):is_successful(success),error_id(id),error(e)
{
	
}

//ErrorMessage::ErrorMessage(const ErrorMessage& error_message)
//{
//	is_successful = error_message.is_successful;
//	error_id = error_message.error_id;
//	error = error_message.error;
//}
void ErrorMessage::SetErrorInfo(bool success, int id, std::string e)
{
	is_successful = success;
	error_id = id;
	error = e;
}

SPHGridData::SPHGridData()
{
	series_griddata.reserve(kSPHParticles * 20);
	material.reserve(kSPHParticles);
	is_last_cycle = false;
}
SPHGridData::~SPHGridData()
{
}
bool SPHGridData::SeriesTo2DMatrix(std::vector<std::vector<double>>& matrix)
{

	return true;
}

//在以空白字符分隔的字符串中找到第一个符合pattern的单词，并返回其首字符位置
//如果没有找到则返回-1
int GetFirstRegexMatchPosition(const string& line,const regex& pattern)
{
	if (line.empty())
	{
		return -1;
	}
	vector<string> split_line;
	SplitString(line, 1, split_line);
	if (split_line.empty())
	{
		return -1;
	}
	int n = split_line.size();
	int i = 0;
	while (i < n)
	{
		if (regex_match(split_line[i], pattern))
		{
			break;
		}
		i++;
	}
	if (i == n)
	{
		return -1;
	}
	size_t pos = line.find(split_line[i]);
	return int(pos);
}

//返回s中第一个非空白字符位置
//若s为空或全是空白字符则返回-1
int GetFirstNonblankCharacterPosition(const std::string& s)
{
	if (s.empty())
	{
		return -1;
	}
	int n = s.size();
	int i = 0;
	for (; i < n; ++i)
	{
		if (!isblank(s[i]))
		{
			break;
		}
	}
	if (i == n)
	{
		return -1;
	}
	return i;
	
}

//获取数据行开头索引，失败则返回-1
int GetIndexFromDataLine(const std::string& line)
{
	if (line.empty())
	{
		return -1;
	}
	regex positive_integer(R"(^[1-9]+\d*$)");
	int n = line.size();
	int i = 0;
	string s;
	for (int j = 0; j < n; ++j)
	{
		if (!isblank(line[j]))
		{
			i = j;
			while (j < n && !isblank(line[j]))
			{
				j++;
			}
			s = line.substr(i, j - i);
			break;
		}
	}
	if (!regex_match(s, positive_integer))
	{
		return -1;
	}
	return stoi(s);
}
//获取数据行中的材料信息
//first_scientific_notation 是首个科学计数法数字的首字符位置
//line具有形式:
//[正整数索引][材料名][科学计数法数值a][科学计数法数值b][科学计数法数值c]......
//不进行格式校验
std::string GetMaterialFromDataLine(const std::string& line, int particle_index_position, int first_scientific_notation_position)
{
	string res;
	if (line.empty())
	{
		return string("");
	}
	int n = int(line.size());
	if (particle_index_position < 0 || particle_index_position >= n || first_scientific_notation_position >= n || first_scientific_notation_position < 0)
	{
		return string("");
	}
	int i = particle_index_position;
	int j = first_scientific_notation_position;
	while (i < j && !isblank(line[i]))
	{
		i++;
	}
	if (i == j)
	{
		return string("");
	}
	return SlimString(line.substr(i, j - i));

}


//进行格式校验
std::string GetMaterialFromDataLine(const std::string& line)
{
	regex sci_real(R"(^[+-]?\d+.\d*E[+-]?\d+$)");
	regex positive_integer(R"(^[1-9]+\d*$)");
	int x = GetFirstRegexMatchPosition(line, positive_integer);
	int y = GetFirstRegexMatchPosition(line, sci_real);
	if (x == -1 || y == -1)
	{
		return string("");
	}
	return GetMaterialFromDataLine(line, x, y);
}

//SlimString removes leading and trailing whitespace characters of string s.
//if s is empty or contains only whitespace characters,we resize s to empty.
string SlimString(const string& line)
{
	string s(line);
	int n = s.size();
	if (n == 0)
	{
		return s;
	}
	if (n == 1 && isspace(s[0]))
	{
		s.resize(0);
		return s;
	}
	int first = 0;
	int last = n - 1;
	for (int i = 0; i < n; ++i)
	{
		first = i;
		if (!isspace(s[i]))
		{
			break;
		}
	}
	for (int i = n - 1; i >= 0; --i)
	{
		last = i;
		if (!isspace(s[i]))
		{
			break;
		}
	}
	if (first > last)
	{
		s.resize(0);
		return s;
	}
	int m = last - first + 1;
	for (int i = 0; i < m; ++i)
	{
		s[i] = s[i + first];
	}
	s.resize(m);
	return s;
}

//两个单词至少相隔blanks个空白字符('\t','\x20')才会被分割
bool SplitString(const string& line, int blanks, vector<string>& res)
{
	if (blanks < 1)
	{
		return false;
	}
	res.resize(0);
	string s = SlimString(line);
	if (s.empty())
	{
		return true;
	}
	int i = 0;
	int j = 0;
	int n = s.size();
	int k = 0;
	while (j < n)
	{
		if (isblank(s[j]))
		{
			while (j < n && isblank(s[j]))
			{
				k++;
				j++;
			}
			if (k >= blanks)
			{
				res.push_back(s.substr(i, j - i - k));
				i = j;
			}
			k = 0;
		}
		if (j == n - 1)
		{
			res.push_back(s.substr(i, j - i + 1));
		}
		j++;
	}
	return true;

}


//通过separator分割字符串，用find函数查找separator
vector<string> SplitString(const string& line, string separator)
{
	vector<string> res;
	string s = SlimString(line);
	int m = separator.size();
	while (!s.empty())
	{
		int pos = s.find(separator);
		if (pos == s.npos)
		{
			res.push_back(s);
			break;
		}
		res.push_back(s.substr(0, pos));

		s = SlimString(s.substr(pos + m));
	}
	return res;
}

std::string GetFirstNonBlankSubString(const std::string& line)
{
	int n = line.size();
	if (n == 0)
	{
		return string("");
	}
	int i = 0;
	while (i < n && isblank(line[i]))
	{
		i++;
	}
	if (i == n)
	{
		return string("");
	}
	int j = i;
	while (j < n && !isblank(line[j]))
	{
		j++;
	}
	return line.substr(i, j - i);
}

//从数据段信息title中获取cycle id
//示例:
//"CYCLE    1046, TIME= 3.302E-03                SUBGRID: OFHC-Al    (SPH          ), IMAX=  1, JMAX= 913104, KMAX=  1 "
int GetCycleIDFromSegmentTitle(const string& title)
{
	string s = SlimString(title);
	int n = s.size();
	if (n == 0)
	{
		return -1;
	}
	if (s.find("CYCLE") != 0)
	{
		return -1;
	}
	int i = 0;
	while (i < n && !isdigit(s[i]))
	{
		i++;
	}
	if (i == n)
	{
		return -1;
	}
	int j = i;
	while (j < n && isdigit(s[j]))
	{
		j++;
	}
	return stoi(s.substr(i, j - i));
}



//从数据段信息title中获取time 
//示例:
//"CYCLE    1046, TIME= 3.302E-03                SUBGRID: OFHC-Al    (SPH          ), IMAX=  1, JMAX= 913104, KMAX=  1 "
double GetTimeFromSegmentTitle(const std::string& title)
{
	int pos = 0;
	if ((pos = title.find("TIME=")) == title.npos)
	{
		return -1.0;
	}
	string s = title.substr(pos + string("TIME=").size());
	string res = GetFirstNonBlankSubString(s);
	if (res.empty())
	{
		return -1.0;
	}
	regex sci_real(R"(^[+-]?\d+.\d*E[+-]?\d+$)");
	if (!regex_match(res, sci_real))
	{
		return -1.0;
	}
	return stod(res);
}

bool ReadCycleTimeFromDataLine(const std::string& line, int& cycle_id, double& cycle_time)
{
	int a = GetCycleIDFromSegmentTitle(line);
	double b = GetTimeFromSegmentTitle(line);
	if (a == -1 || b == -1.0)
	{
		return false;
	}
	cycle_id = a;
	cycle_time = b;
	return true;
}



ErrorMessage SPHGridData::AppendSeriesData(const string& line, int pos)
{
	
	int n = line.size();
	if (pos < 0 || pos >= n)
	{
		char str[kSprintfBufferSize];
		sprintf_s(str, kSprintfBufferSize, "%s,%d. Error input.", __FUNCTION__, __LINE__);
		return ErrorMessage(false, -1, string(str));
	}
	string s = SlimString(line.substr(pos));
	n = s.size();
	if (n == 0)
	{
		char str[kSprintfBufferSize];
		sprintf_s(str, kSprintfBufferSize, "%s,%d. Error input.", __FUNCTION__, __LINE__);
		return ErrorMessage(false, -1, string(str));
	}
	int i = 0;
	int j = 0;

	while (j < n)
	{
		if (isblank(s[j]))
		{
			double a = 0.0;
			try
			{
				a = stod(s.substr(i, j - i));
			}
			catch (const std::exception& e)
			{
				char str[kSprintfBufferSize];
				sprintf_s(str, kSprintfBufferSize, "%s,%d. %s .", __FUNCTION__, __LINE__, e.what());
				return ErrorMessage(false, -1, string(str));
			}
			series_griddata.push_back(a);
			while (j < n && isblank(s[j]))
			{
				j++;
			}
			i = j;
		}
		j++;
	}
	ErrorMessage message;
	return message;
}



//ReadFirstCycle read material info,variables name and grid data
//the next first nonblank line of ptr_autodyn must match with regex segment_title
//此函数要求文件输入流读出的第一个非空行必须满足data_start正则表达式
//此函数读取材料和变量名
//此函数只读一个cycle的数据，判断这个cycle读完的标志为下列两个之一
//(1)segment title的cycle id 改变
//(2)到达文件末尾，没有下一个segment title行
//由于每个cycle的第一个section才有材料信息，所以第一个section要特别处理
//假定同一个section，同一个变量的不同SPH粒子的数据是严格对齐的，因此只需要获取一行的数值数据起始位置，就可以确定同一个section其他粒子数据的起始位置
ErrorMessage SPHGridData::ReadFirstCycle(std::ifstream& prt_autodyn, const SPHGridDataInfo& file_header)
{
	ResetSPHGridData();//重置所有数据，防止上一次读取的数据的影响;因为使用了push_back，其不会清除之前的数据.
	//streampos initial_stream_position = ptr_autodyn.tellg();
	ErrorMessage message;
	regex segment_title(R"(^CYCLE.*,.*TIME=.*IMAX=.*JMAX=.*KMAX=.*$)");
	regex sci_real(R"(^[+-]?\d+.\d*E[+-]?\d+$)");
	regex positive_integer(R"(^[1-9]+\d*$)");
	is_last_cycle = false;//标记当前cycle是不是文件最后一个cycle
	//要求读入的第一个非空白字符行必须满足regex segment_title
	bool is_first_nonblank_line_of_cycle = true;
	
	//
	bool is_header_of_first_segment_of_section = true;
	//the id of first section in a cycle is zero.
	int section_id = 0;

	//the id of dataline is equal to SPH particle id
	//if particle_id == SPHGridDataInfo::total_number_nodes, the stream arrives at the end of current section  
	int particle_id = 0;

	int N = file_header.total_number_nodes;
	sph_particles_amount = N;
	
	//为了下一个cycle读取，需要将位置复位到下一个cycle的开头
	streampos last_line_pos = prt_autodyn.tellg();
	//
	while (!prt_autodyn.eof())
	{
		last_line_pos = prt_autodyn.tellg();
		string line;
		getline(prt_autodyn, line);
		string slimline = SlimString(line);
		if (slimline.empty())
		{
			continue;
		}
		if (isdigit(slimline[0]))//首字符是数字表示该行一定是SPH粒子数据行
		{
			particle_id++;
			if (particle_id == 1)//section的第一个SPH粒子数据行需要特别处理，来获得首个科学计数法数值的位置并记录在scientific_notation_position里供后续SPH粒子数据行读取使用
			{
				int pos = GetFirstRegexMatchPosition(line, sci_real);
				if (pos == -1)//数据行中应该出现科学计数法却没有，说明读取出现问题，退出
				{
					char temp_str[kSprintfBufferSize];
					sprintf_s(temp_str, kSprintfBufferSize, "%s %d Error reading SPH partice data line:cycle_id=%d, section_id = %d, particle_id = %d .",
						__FUNCTION__, __LINE__, cycle_id, section_id, particle_id);
					message.SetErrorInfo(false, -2, string(temp_str));
					return message;
				}
				scientific_notation_position.push_back(pos);
				variable_amount_first_dataline.push_back(GetNumberOfWords(line, pos));
			}
			if (section_id == 0)//section_id == 0 为true则当前SPH粒子数据行位于第一个section，包含材料信息
			{
				int pos_sci_real = scientific_notation_position[0];
				int pos_index = GetFirstNonblankCharacterPosition(line);
				string particle_material = GetMaterialFromDataLine(line, pos_index, pos_sci_real);
				if (particle_material.empty())
				{
					char temp_str[kSprintfBufferSize];
					sprintf_s(temp_str, kSprintfBufferSize, "%s %d Error reading SPH partice material:cycle_id=%d, section_id = %d, particle_id = %d .",
						__FUNCTION__, __LINE__, cycle_id, section_id, particle_id);
					message.SetErrorInfo(false, -2, string(temp_str));
					return message;
				}
				material.push_back(particle_material);
			}
			int pos = scientific_notation_position[section_id];
			message = AppendSeriesData(line, pos);
			if (!message.is_successful)
			{
				return message;
			}
#ifdef _DEBUG
			if (particle_id % 50000 == 0 || particle_id == N)
			{
				cout << "particle_id = " << particle_id << endl;
			}
#endif // DEBUG
			
		}
		else//首字符不是数字，则属于segment header
		{
			if (is_first_nonblank_line_of_cycle)//对遇到的第一个非空行进行校验，不满足regex就返回错误message;满足regex则继续读两个非空行获取变量名和一些变量
			{
				is_first_nonblank_line_of_cycle = false;//保证这个if里面的语句只会在调用时运行一次，读取后续section时不会进来
				if (!regex_match(slimline, segment_title))
				{
					message.SetErrorInfo(false, -2, string("Error reading first cycle title."));
					return message;
				}
				vector<string> segment_header;
				segment_header.push_back(slimline);
				if (!ReadNonblankLines(prt_autodyn, segment_header, 2))
				{
					break;
				}
				cycle_id = GetCycleIDFromSegmentTitle(segment_header[0]);
				cycle_time = GetTimeFromSegmentTitle(segment_header[0]);
				vector<string> split_line;
				SplitString(segment_header[2], 2, split_line);
				int num_var = int(split_line.size()) - 2;//减去粒子索引名和"MATERIAL"名
				variable_amount.push_back(num_var);
				for (int i = 2; i < num_var + 2; ++i)//非第一个section则变量名从1开始，对第一个section变量名从2开始，粒子索引变量名始终位于0
				{
					variable_name.push_back(split_line[i]);
				}
			}

			if (particle_id == N)//particle_id == N 为true 说明当前line是一个section结束后遇到的第一个非空行,再连读两个非空行，通过这三个非空行判断文件读取状态
			{
				if (!regex_match(slimline, segment_title))//不匹配则表示后续的行不是segment了，可以退出循环
				{
					is_last_cycle = true;
					break;
				}
				int cycle_id_current = GetCycleIDFromSegmentTitle(slimline);
				if (cycle_id_current != cycle_id)//意即后续行是下一个cycle的内容，则需要把文件指针复位到下一个cycle的title的开始，然后退出循环
				{
					prt_autodyn.seekg(last_line_pos, ios::beg);
					break;
				}
				vector<string> segment_header;
				segment_header.push_back(slimline);
				if (!ReadNonblankLines(prt_autodyn, segment_header, 2))
				{
					is_last_cycle = true;
					break;
				}
				vector<string> split_line;
				SplitString(segment_header[2], 2, split_line);
				int num_var = int(split_line.size()) - 1;//减去粒子索引名
				variable_amount.push_back(num_var);
				for (int i = 1; i < num_var + 1; ++i)//非第一个section则变量名从1开始，对第一个section变量名从2开始，粒子索引变量名始终位于0
				{
					variable_name.push_back(split_line[i]);
				}
				section_id++;//能运行到此处，则说明后续内容是本cycle的section，即发现了本cycle的新的section
				particle_id = 0;//为读取下一个section的dataline，将particle_id置为零
			}

		}

	}
	if (prt_autodyn.eof())//如果是因为到达文件末尾了才退出循环则需要把is_last_cycle置为true
	{
		is_last_cycle = true;
		return message;
	}

	return message;
}




//ReadNextCycle only read grid data
//the next first nonblank line of ptr_autodyn must match with regex segment_title
ErrorMessage SPHGridData::ReadNextCycle(std::ifstream& prt_autodyn)
{
	series_griddata.resize(0);//重置，防止之前数据的干扰
	//streampos initial_stream_position = ptr_autodyn.tellg();
	ErrorMessage message;
	regex segment_title(R"(^CYCLE.*,.*TIME=.*IMAX=.*JMAX=.*KMAX=.*$)");
	regex sci_real(R"(^[+-]?\d+.\d*E[+-]?\d+$)");
	regex positive_integer(R"(^[1-9]+\d*$)");
	is_last_cycle = false;//标记当前cycle是不是文件最后一个cycle
	//要求读入的第一个非空白字符行必须满足regex segment_title
	bool is_first_nonblank_line_of_cycle = true;

	//
	bool is_header_of_first_segment_of_section = true;
	//the id of first section in a cycle is zero.
	int section_id = 0;

	//the id of dataline is equal to SPH particle id
	//if particle_id == SPHGridDataInfo::total_number_nodes, the stream arrives at the end of current section  
	int particle_id = 0;
	int N = sph_particles_amount;
	if (N <= 0)
	{
		char temp_str[kSprintfBufferSize];
		sprintf_s(temp_str, kSprintfBufferSize, "%s %d,Error SPH particles amount .", __FUNCTION__, __LINE__);
		message.SetErrorInfo(false, -2, string(temp_str));
		return message;
	}
	//为了下一个cycle读取，需要将位置复位到下一个cycle的开头
	streampos last_line_pos = prt_autodyn.tellg();
	//
	while (!prt_autodyn.eof())
	{
		last_line_pos = prt_autodyn.tellg();
		string line;
		getline(prt_autodyn, line);
		string slimline = SlimString(line);
		if (slimline.empty())
		{
			continue;
		}
		if (isdigit(slimline[0]))//首字符是数字表示该行一定是SPH粒子数据行
		{
			particle_id++;
			int pos = scientific_notation_position[section_id];
			message = AppendSeriesData(line, pos);
			if (!message.is_successful)
			{
				return message;
			}
			if (particle_id % 50000 == 0 || particle_id == N)
			{
				cout << "particle_id = " << particle_id << endl;
			}

		}
		else//首字符不是数字，则属于segment header
		{
			if (is_first_nonblank_line_of_cycle)//对遇到的第一个非空行进行校验，不满足regex就返回错误message;满足regex则继续读两个非空行获取变量名和一些变量
			{
				is_first_nonblank_line_of_cycle = false;//保证这个if里面的语句只会在调用时运行一次，读取后续section时不会进来
				if (!regex_match(slimline, segment_title))
				{
					message.SetErrorInfo(false, -2, string("Error reading first cycle title."));
					return message;
				}
				vector<string> segment_header;
				segment_header.push_back(slimline);
				if (!ReadNonblankLines(prt_autodyn, segment_header, 2))
				{
					break;
				}
				cycle_id = GetCycleIDFromSegmentTitle(segment_header[0]);
				cycle_time = GetTimeFromSegmentTitle(segment_header[0]);
			}

			if (particle_id == N)//particle_id == N 为true 说明当前line是一个section结束后遇到的第一个非空行,再连读两个非空行，通过这三个非空行判断文件读取状态
			{
				if (!regex_match(slimline, segment_title))//不匹配则表示后续的行不是segment了，可以退出循环
				{
					is_last_cycle = true;
					break;
				}
				int cycle_id_current = GetCycleIDFromSegmentTitle(slimline);
				if (cycle_id_current != cycle_id)//意即后续行是下一个cycle的内容，则需要把文件指针复位到下一个cycle的title的开始，然后退出循环
				{
					prt_autodyn.seekg(last_line_pos, ios::beg);
					break;
				}
				vector<string> segment_header;
				segment_header.push_back(slimline);
				if (!ReadNonblankLines(prt_autodyn, segment_header, 2))
				{
					is_last_cycle = true;
					break;
				}
				section_id++;//能运行到此处，则说明后续内容是本cycle的section，即发现了本cycle的新的section
				particle_id = 0;//为读取下一个section的dataline，将particle_id置为零
			}

		}

	}
	if (prt_autodyn.eof())//如果是因为到达文件末尾了才退出循环则需要把is_last_cycle置为true
	{
		is_last_cycle = true;
		return message;
	}

	return message;
}


//Read file header
std::string ReadFileHeader(std::ifstream& ptr_in_fstream, SPHGridDataInfo& file_header)
{
	const vector<regex> regex_autodyn_prt = {
			regex(R"(^Length\x20+:\x20+.+$)"),regex(R"(^Mass\x20+:\x20+.+$)"),regex(R"(^Time\x20+:\x20+.+$)"),
			regex(R"(^MATERIAL\x20NAME:\x20.+$)"),regex(R"(^Reference\x20density\x20+.+$)"),
			regex(R"(^PART\x20+PROCESSOR\x20+IMAX\x20+JMAX\x20+KMAX$)"),//此处匹配后紧邻的下一个非空行后三个整数对应I_MAX,J_MAX,K_MAX
			regex(R"(^TOTAL\x20+NUMBER\x20+NODES:\x20+[1-9]\d*$)"),
			regex(R"(^SPH\x20OBJECTS$)"),//此处匹配后，下面首次出现的以1,2,3, ...开头的行表示object names 相关数据
			regex(R"(^PART\x20+I\x20+BEGIN\x20+I\x20+END\x20+J\x20+BEGIN\x20+J\x20+END\x20+K\x20+BEGIN\x20+K\x20+END\x20+ORDER$)"),//此处匹配成功后
			//后面紧邻的连续行表示PART，直到出现一个空行停止，假设只有一个PART
	};
	regex data_start(R"(^CYCLE.*,.*TIME=.*IMAX=.*JMAX=.*KMAX=.*$)");
	istringstream iss;
	string line;
	vector<uint32_t> match_frequency(9, 0);
	streampos last_line_pos = ptr_in_fstream.tellg();
	streampos cur_line_pos = ptr_in_fstream.tellg();
	while (!ptr_in_fstream.eof())
	{
		last_line_pos = ptr_in_fstream.tellg();
		getline(ptr_in_fstream, line);
		string slim_line = SlimString(line);
		if (regex_match(slim_line, regex_autodyn_prt[0]))
		{
			cout << line << endl;
			match_frequency[0]++;
			int colon_pos = slim_line.find(':');
			string temp = slim_line.substr(colon_pos + 1);
			file_header.units.push_back(SlimString(temp));
			continue;
		}
		if (regex_match(slim_line, regex_autodyn_prt[1]))
		{
			cout << line << endl;
			match_frequency[1]++;
			int colon_pos = slim_line.find(':');
			string temp = slim_line.substr(colon_pos + 1);
			file_header.units.push_back(SlimString(temp));
			continue;
		}
		if (regex_match(slim_line, regex_autodyn_prt[2]))
		{
			cout << line << endl;
			match_frequency[2]++;
			int colon_pos = slim_line.find(':');
			string temp = slim_line.substr(colon_pos + 1);
			file_header.units.push_back(SlimString(temp));
			continue;
		}
		if (regex_match(slim_line, regex_autodyn_prt[3]))
		{
			cout << line << endl;
			match_frequency[3]++;
			int colon_pos = slim_line.find(':');
			string temp = slim_line.substr(colon_pos + 1);
			file_header.material_names.push_back(SlimString(temp));
			continue;
		}
		if (regex_match(slim_line, regex_autodyn_prt[4]))
		{
			cout << line << endl;
			match_frequency[4]++;
			int left_bracket_pos = slim_line.find('(');
			int right_bracket_pos = slim_line.find(')');
			string temp = slim_line.substr(left_bracket_pos + 1, right_bracket_pos - left_bracket_pos - 1);
			file_header.density_unit.push_back(SlimString(temp));
			temp = slim_line.substr(right_bracket_pos + 1);
			iss.str(temp);
			double density;
			iss >> density;
			file_header.reference_density.push_back(density);
			iss.clear();
			iss.str("");
			continue;
		}
		if (regex_match(slim_line, regex_autodyn_prt[5]))
		{
			cout << line << endl;
			match_frequency[5]++;
			string temp;
			while (true)
			{
				getline(ptr_in_fstream, line);
				slim_line = SlimString(line);
				if (!slim_line.empty())
				{
					break;
				}
			}
			cout << __LINE__ << " : " << line << endl;
			iss.clear();
			iss.str(slim_line);
			vector<string> split_line;
			while (iss >> temp)
			{
				split_line.push_back(temp);
			}
			iss.clear();

			int split_line_size = split_line.size();
			cout << "split_line_size = " << split_line_size << endl;
			int M;
			iss.str(split_line[split_line_size - 1]);
			iss >> M;
			file_header.I_MAX = M;
			iss.clear();
			iss.str(split_line[split_line_size - 2]);
			iss >> M;
			file_header.J_MAX = M;
			iss.clear();
			iss.str(split_line[split_line_size - 3]);
			iss >> M;
			file_header.K_MAX = M;
			iss.clear();
			continue;
		}
		if (regex_match(slim_line, regex_autodyn_prt[6]))
		{
			cout << line << endl;
			match_frequency[6]++;
			string temp;
			int colon_pos = slim_line.find(':');
			temp = SlimString(slim_line.substr(colon_pos + 1));
			iss.str(temp);
			iss >> file_header.total_number_nodes;
			iss.clear();
		}
		if (regex_match(slim_line, data_start))
		{
			cout << __LINE__ << " : " << line << endl;
			break;
		}
	}
	file_header.first_cycle_start = last_line_pos;
	return line;
}

//因为读取数据填充vector时使用了push_back,如果不清空数据,则导致下一个cycle的数据附加到上一个cycle数据的后面
void SPHGridData::ResetSPHGridData()
{
	scientific_notation_position.resize(0);
	series_griddata.resize(0);
	variable_amount.resize(0);
	variable_amount_first_dataline.resize(0);
	variable_name.resize(0);
	material.resize(0);
	is_last_cycle = false;
	cycle_id = -1;
	cycle_time = -1.0;
	sph_particles_amount = -1;
}



//read k nonblank slimed lines from file stream 
//if the number of  nonblank slimed lines that can be readout is small than k, return false
bool ReadNonblankLines(std::ifstream& prt_autodyn, std::vector<std::string>& res, int k)
{
	if (k <= 0)
	{
		return false;
	}
	int n = res.size();
	int i = 0;
	while (!prt_autodyn.eof() && i < k)
	{
		string line;
		getline(prt_autodyn, line);
		string slimline = SlimString(line);
		if (!slimline.empty())
		{
			i++;
			res.push_back(slimline);
		}
	}
	if (i < k)
	{
		res.resize(n);
		return false;
	}
	return true;
}

//获取从pos开始的子串的word数量
int GetNumberOfWords(const std::string& line, int pos)
{
	string s = SlimString(line.substr(pos));
	vector<string> res;
	SplitString(s, 1, res);
	return res.size();
}