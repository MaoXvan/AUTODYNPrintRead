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

//���Կհ��ַ��ָ����ַ������ҵ���һ������pattern�ĵ��ʣ������������ַ�λ��
//���û���ҵ��򷵻�-1
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

//����s�е�һ���ǿհ��ַ�λ��
//��sΪ�ջ�ȫ�ǿհ��ַ��򷵻�-1
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

//��ȡ�����п�ͷ������ʧ���򷵻�-1
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
//��ȡ�������еĲ�����Ϣ
//first_scientific_notation ���׸���ѧ���������ֵ����ַ�λ��
//line������ʽ:
//[����������][������][��ѧ��������ֵa][��ѧ��������ֵb][��ѧ��������ֵc]......
//�����и�ʽУ��
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


//���и�ʽУ��
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

//���������������blanks���հ��ַ�('\t','\x20')�Żᱻ�ָ�
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


//ͨ��separator�ָ��ַ�������find��������separator
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

//�����ݶ���Ϣtitle�л�ȡcycle id
//ʾ��:
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



//�����ݶ���Ϣtitle�л�ȡtime 
//ʾ��:
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
//�˺���Ҫ���ļ������������ĵ�һ���ǿ��б�������data_start������ʽ
//�˺�����ȡ���Ϻͱ�����
//�˺���ֻ��һ��cycle�����ݣ��ж����cycle����ı�־Ϊ��������֮һ
//(1)segment title��cycle id �ı�
//(2)�����ļ�ĩβ��û����һ��segment title��
//����ÿ��cycle�ĵ�һ��section���в�����Ϣ�����Ե�һ��sectionҪ�ر���
//�ٶ�ͬһ��section��ͬһ�������Ĳ�ͬSPH���ӵ��������ϸ����ģ����ֻ��Ҫ��ȡһ�е���ֵ������ʼλ�ã��Ϳ���ȷ��ͬһ��section�����������ݵ���ʼλ��
ErrorMessage SPHGridData::ReadFirstCycle(std::ifstream& prt_autodyn, const SPHGridDataInfo& file_header)
{
	ResetSPHGridData();//�����������ݣ���ֹ��һ�ζ�ȡ�����ݵ�Ӱ��;��Ϊʹ����push_back���䲻�����֮ǰ������.
	//streampos initial_stream_position = ptr_autodyn.tellg();
	ErrorMessage message;
	regex segment_title(R"(^CYCLE.*,.*TIME=.*IMAX=.*JMAX=.*KMAX=.*$)");
	regex sci_real(R"(^[+-]?\d+.\d*E[+-]?\d+$)");
	regex positive_integer(R"(^[1-9]+\d*$)");
	is_last_cycle = false;//��ǵ�ǰcycle�ǲ����ļ����һ��cycle
	//Ҫ�����ĵ�һ���ǿհ��ַ��б�������regex segment_title
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
	
	//Ϊ����һ��cycle��ȡ����Ҫ��λ�ø�λ����һ��cycle�Ŀ�ͷ
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
		if (isdigit(slimline[0]))//���ַ������ֱ�ʾ����һ����SPH����������
		{
			particle_id++;
			if (particle_id == 1)//section�ĵ�һ��SPH������������Ҫ�ر���������׸���ѧ��������ֵ��λ�ò���¼��scientific_notation_position�﹩����SPH���������ж�ȡʹ��
			{
				int pos = GetFirstRegexMatchPosition(line, sci_real);
				if (pos == -1)//��������Ӧ�ó��ֿ�ѧ������ȴû�У�˵����ȡ�������⣬�˳�
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
			if (section_id == 0)//section_id == 0 Ϊtrue��ǰSPH����������λ�ڵ�һ��section������������Ϣ
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
		else//���ַ��������֣�������segment header
		{
			if (is_first_nonblank_line_of_cycle)//�������ĵ�һ���ǿ��н���У�飬������regex�ͷ��ش���message;����regex������������ǿ��л�ȡ��������һЩ����
			{
				is_first_nonblank_line_of_cycle = false;//��֤���if��������ֻ���ڵ���ʱ����һ�Σ���ȡ����sectionʱ�������
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
				int num_var = int(split_line.size()) - 2;//��ȥ������������"MATERIAL"��
				variable_amount.push_back(num_var);
				for (int i = 2; i < num_var + 2; ++i)//�ǵ�һ��section���������1��ʼ���Ե�һ��section��������2��ʼ����������������ʼ��λ��0
				{
					variable_name.push_back(split_line[i]);
				}
			}

			if (particle_id == N)//particle_id == N Ϊtrue ˵����ǰline��һ��section�����������ĵ�һ���ǿ���,�����������ǿ��У�ͨ���������ǿ����ж��ļ���ȡ״̬
			{
				if (!regex_match(slimline, segment_title))//��ƥ�����ʾ�������в���segment�ˣ������˳�ѭ��
				{
					is_last_cycle = true;
					break;
				}
				int cycle_id_current = GetCycleIDFromSegmentTitle(slimline);
				if (cycle_id_current != cycle_id)//�⼴����������һ��cycle�����ݣ�����Ҫ���ļ�ָ�븴λ����һ��cycle��title�Ŀ�ʼ��Ȼ���˳�ѭ��
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
				int num_var = int(split_line.size()) - 1;//��ȥ����������
				variable_amount.push_back(num_var);
				for (int i = 1; i < num_var + 1; ++i)//�ǵ�һ��section���������1��ʼ���Ե�һ��section��������2��ʼ����������������ʼ��λ��0
				{
					variable_name.push_back(split_line[i]);
				}
				section_id++;//�����е��˴�����˵�����������Ǳ�cycle��section���������˱�cycle���µ�section
				particle_id = 0;//Ϊ��ȡ��һ��section��dataline����particle_id��Ϊ��
			}

		}

	}
	if (prt_autodyn.eof())//�������Ϊ�����ļ�ĩβ�˲��˳�ѭ������Ҫ��is_last_cycle��Ϊtrue
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
	series_griddata.resize(0);//���ã���ֹ֮ǰ���ݵĸ���
	//streampos initial_stream_position = ptr_autodyn.tellg();
	ErrorMessage message;
	regex segment_title(R"(^CYCLE.*,.*TIME=.*IMAX=.*JMAX=.*KMAX=.*$)");
	regex sci_real(R"(^[+-]?\d+.\d*E[+-]?\d+$)");
	regex positive_integer(R"(^[1-9]+\d*$)");
	is_last_cycle = false;//��ǵ�ǰcycle�ǲ����ļ����һ��cycle
	//Ҫ�����ĵ�һ���ǿհ��ַ��б�������regex segment_title
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
	//Ϊ����һ��cycle��ȡ����Ҫ��λ�ø�λ����һ��cycle�Ŀ�ͷ
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
		if (isdigit(slimline[0]))//���ַ������ֱ�ʾ����һ����SPH����������
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
		else//���ַ��������֣�������segment header
		{
			if (is_first_nonblank_line_of_cycle)//�������ĵ�һ���ǿ��н���У�飬������regex�ͷ��ش���message;����regex������������ǿ��л�ȡ��������һЩ����
			{
				is_first_nonblank_line_of_cycle = false;//��֤���if��������ֻ���ڵ���ʱ����һ�Σ���ȡ����sectionʱ�������
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

			if (particle_id == N)//particle_id == N Ϊtrue ˵����ǰline��һ��section�����������ĵ�һ���ǿ���,�����������ǿ��У�ͨ���������ǿ����ж��ļ���ȡ״̬
			{
				if (!regex_match(slimline, segment_title))//��ƥ�����ʾ�������в���segment�ˣ������˳�ѭ��
				{
					is_last_cycle = true;
					break;
				}
				int cycle_id_current = GetCycleIDFromSegmentTitle(slimline);
				if (cycle_id_current != cycle_id)//�⼴����������һ��cycle�����ݣ�����Ҫ���ļ�ָ�븴λ����һ��cycle��title�Ŀ�ʼ��Ȼ���˳�ѭ��
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
				section_id++;//�����е��˴�����˵�����������Ǳ�cycle��section���������˱�cycle���µ�section
				particle_id = 0;//Ϊ��ȡ��һ��section��dataline����particle_id��Ϊ��
			}

		}

	}
	if (prt_autodyn.eof())//�������Ϊ�����ļ�ĩβ�˲��˳�ѭ������Ҫ��is_last_cycle��Ϊtrue
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
			regex(R"(^PART\x20+PROCESSOR\x20+IMAX\x20+JMAX\x20+KMAX$)"),//�˴�ƥ�����ڵ���һ���ǿ��к�����������ӦI_MAX,J_MAX,K_MAX
			regex(R"(^TOTAL\x20+NUMBER\x20+NODES:\x20+[1-9]\d*$)"),
			regex(R"(^SPH\x20OBJECTS$)"),//�˴�ƥ��������״γ��ֵ���1,2,3, ...��ͷ���б�ʾobject names �������
			regex(R"(^PART\x20+I\x20+BEGIN\x20+I\x20+END\x20+J\x20+BEGIN\x20+J\x20+END\x20+K\x20+BEGIN\x20+K\x20+END\x20+ORDER$)"),//�˴�ƥ��ɹ���
			//������ڵ������б�ʾPART��ֱ������һ������ֹͣ������ֻ��һ��PART
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

//��Ϊ��ȡ�������vectorʱʹ����push_back,������������,������һ��cycle�����ݸ��ӵ���һ��cycle���ݵĺ���
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

//��ȡ��pos��ʼ���Ӵ���word����
int GetNumberOfWords(const std::string& line, int pos)
{
	string s = SlimString(line.substr(pos));
	vector<string> res;
	SplitString(s, 1, res);
	return res.size();
}