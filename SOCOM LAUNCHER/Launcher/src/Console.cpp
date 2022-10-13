#include "../include//Console.hpp"
#include "../include//Launcher.hpp"

namespace SOCOMLauncher {
	Console::Console()
	{
		/// Create Folders if they do not exist
		if (!std::filesystem::exists(xorstr_("LauncherExpansions")))
			std::filesystem::create_directory(xorstr_("LauncherExpansions"));

		/// Logs directory
		if (!std::filesystem::exists(xorstr_("LauncherExpansions\\logs")))
			std::filesystem::create_directory(xorstr_("LauncherExpansions\\logs"));

		return;
	}

	void Console::InitializeConsole(const char* ConsoleName)
	{
		AllocConsole();
		g_Handle = GetStdHandle(STD_OUTPUT_HANDLE);
		g_hWnd = GetConsoleWindow();
		freopen_s(&stream_in, "CONIN$", "r", stdin);
		freopen_s(&stream_out, "CONOUT$", "w", stdout);
		freopen_s(&stream_error, "CONOUT$", "w", stderr);
		SetConsoleTitleA(ConsoleName);
		ShowWindow(g_hWnd, SW_SHOW);
		return;
	}

	void Console::printdbg(const char* Text, int Color, ...)
	{
		SetConsoleTextAttribute(g_Handle, Color);
		va_list arg;
		va_start(arg, Color);
		vfprintf(stream_out, Text, arg);
		va_end(arg);
		SetConsoleTextAttribute(g_Handle, color.DEFAULT);
		return;
	}

	void Console::scandbg(const char* Text, ...)
	{
		va_list arg;
		va_start(arg, Text);
		vfscanf(stream_in, Text, arg);
		va_end(arg);
		return;
	}

	bool Console::openFile()
	{
		//	CREATE FILE OBJECT INSTANCE
		HRESULT f_SysHr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
		if (FAILED(f_SysHr)) {
			return FALSE;
		}

		// CREATE FileOpenDialog OBJECT
		IFileOpenDialog* f_FileSystem;
		f_SysHr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_IFileOpenDialog, reinterpret_cast<void**>(&f_FileSystem));
		if (FAILED(f_SysHr)) {
			CoUninitialize();
			return FALSE;
		}

		//	SHOW OPEN FILE DIALOG WINDOW
		f_SysHr = f_FileSystem->Show(NULL);
		if (FAILED(f_SysHr)) {
			f_FileSystem->Release();
			CoUninitialize();
			return FALSE;
		}

		//	RETRIEVE FILE NAME FROM THE SELECTED ITEM
		IShellItem* f_Files;
		f_SysHr = f_FileSystem->GetResult(&f_Files);
		if (FAILED(f_SysHr)) {
			f_FileSystem->Release();
			CoUninitialize();
			return FALSE;
		}

		//	STORE AND CONVERT THE FILE NAME
		PWSTR f_Path;
		f_SysHr = f_Files->GetDisplayName(SIGDN_FILESYSPATH, &f_Path);
		if (FAILED(f_SysHr)) {
			f_Files->Release();
			f_FileSystem->Release();
			CoUninitialize();
			return FALSE;
		}

		MessageBox(NULL, f_Path, xorstr_(L"SELECTED FILE"), NULL);
		std::wstring path(f_Path);
		std::string c(path.begin(), path.end());
		result = c;

		//	SUCCESS, CLEAN UP
		CoTaskMemFree(f_Path);
		f_Files->Release();
		f_FileSystem->Release();
		CoUninitialize();
		return TRUE;
	}

	bool Console::writeFile(const char* Path, const char* Text, unsigned int Length, DWORD* out)
	{
		DWORD temp = 0;
		if (out == NULL)
			out = &temp;
		*out = 0;
		auto handle = CreateFileA(Path, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (handle)
		{
			bool result = WriteFile(handle, Text, Length, out, NULL) && *out > 0;
			CloseHandle(handle);
			return result;
		}
		return false;
	}

	bool Console::readFile(const char* Path, char* Text, unsigned int Length, DWORD* out)
	{
		DWORD temp = 0;
		if (out == NULL)
			out = &temp;
		*out = 0;
		auto handle = CreateFileA(Path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (handle)
		{
			bool result = ReadFile(handle, Text, Length, out, NULL) && *out > 0;
			CloseHandle(handle);
			return result;
		}
		return false;
	}

	/// <summary>
	/// Log Toggle Events
	/// </summary>
	/// <param name="EXAMPLE: ">g_Console->LogEvent("[+] MENU:: ESP - ", g_GameVariables->bESP);</param>
	void Console::LogEvent(std::string TEXT, bool FLAG)
	{
		std::string output;
		std::string append;
		int color;
		switch (FLAG) {
		case(TRUE):
			output = " [ON]\n";
			color = g_Console->color.green;
			append = TEXT + output;
			printdbg(append.c_str(), color);
			break;
		case(FALSE):
			output = " [OFF]\n";
			color = g_Console->color.red;
			append = TEXT + output;
			printdbg(append.c_str(), color);
			break;
		}
	}

	void Console::SaveCFG()
	{
		std::ofstream fout;
		fout.open(m_iniPath, std::ios::trunc);
		fout << g_LauncherVariables->m_CustomColors.x << "\n";
		fout << g_LauncherVariables->m_CustomColors.y << "\n";
		fout << g_LauncherVariables->m_CustomColors.z << "\n";
		fout << g_LauncherVariables->m_CustomColors.w << std::endl;
		fout.close();
		return;
	}

	void Console::LoadCFG()
	{
		//	Check if file exists
		if (!std::filesystem::exists(m_iniPath)) {
			MessageBoxA(NULL, xorstr_("FAILED TO LOAD USER SETTINGS!\nFILE: 'settings.ini'\nLOCATION: 'LauncherExpansions\\settings.ini'"), xorstr_("USER THEME ERROR"), MB_ICONERROR);
			return;
		}

		std::ifstream fin;
		std::string Word = "";
		fin.open(m_iniPath, std::ifstream::in);
		for (int i = 0; i <= 3; i++) {
			fin >> Word;
			g_LauncherVariables->cfgColors.push_back(std::stof(Word));
		}
		g_LauncherVariables->m_CustomColors.x = g_LauncherVariables->cfgColors[0];
		g_LauncherVariables->m_CustomColors.y = g_LauncherVariables->cfgColors[1];
		g_LauncherVariables->m_CustomColors.z = g_LauncherVariables->cfgColors[2];
		g_LauncherVariables->m_CustomColors.w = g_LauncherVariables->cfgColors[3];
		fin.close();
		return;
	}

	const wchar_t* Console::Char2WChar(const char* in)
	{
		if (in == NULL) {
			wchar_t* Out = NULL;
			return Out;
		}
		wchar_t* Out = new wchar_t[strlen(in) + 1];
		mbstowcs_s(NULL, Out, strlen(in) + 1, in, strlen(in));
		return Out;
	}

	const char* Console::WChar2Char(const wchar_t* in)
	{
		if (in == L"") {
			const char* e_out = NULL;
			return e_out;
		}
		std::wstring convert(in);
		std::string out(convert.begin(), convert.end());
		return out.c_str();
	}

	const wchar_t* Console::String2WChar(std::string in)
	{
		if (in == "") {
			wchar_t* out = NULL;
			return out;
		}
		std::wstring convert = std::wstring(in.begin(), in.end());
		const wchar_t* out = convert.c_str();
		return out;
	}

	std::string Console::CurrentProcName(CHAR* path)
	{
		//  Obtain full module path
		GetModuleFileNameA(NULL, path, MAX_PATH);
		const std::string fullpath = path;

		//  Search for last backslash in the index
		const size_t lastSlashIndex = fullpath.find_last_of("/\\");

		//  remove everything except the filename and return
		return fullpath.substr(lastSlashIndex + 1);
	}

	std::string Console::GetHTTP(std::string URL, bool Filter)
	{
		std::string RESULT{};
		const int size = 2048;
		char buf[size];
		DWORD length;

		//	Establish a connection to the internet with Mozilla as our user agent
		HINTERNET internet = InternetOpenA(xorstr_("Mozilla/5.0"), INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, NULL);
		if (!internet)
			return "";

		//	Scrape data from the desired webpage
		HINTERNET response = InternetOpenUrlA(internet, URL.c_str(), NULL, NULL, NULL, NULL);
		if (!response) {
			InternetCloseHandle(internet);
			return "";
		}
		InternetReadFile(response, buf, size, &length);
		InternetCloseHandle(response);
		InternetCloseHandle(internet);

		//	Clean our string 
		if (Filter) {
			size_t count = 0;
			const std::string& FilterResult = xorstr_("null");
			const std::string& newItem = xorstr_("\r\n");
			while (count = RESULT.find(FilterResult, count) != std::string::npos) {
				RESULT.replace(count, FilterResult.length(), newItem);
				count += newItem.length();
			}
		}
		else RESULT.append(buf, length);

		return RESULT;
	}

	void Console::DestroyConsole()
	{
		fclose(stream_in);
		fclose(stream_out);
		fclose(stream_error);
		DestroyWindow(g_hWnd);
		FreeConsole();
		return;
	}
}