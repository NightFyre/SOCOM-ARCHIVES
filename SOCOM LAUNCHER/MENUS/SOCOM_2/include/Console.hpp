#pragma once
#include "../helper.hpp"
namespace SOCOM2 {
	struct Colors {
		int dark_blue = 1;
		int dark_green = 2;
		int dark_teal = 3;
		int dark_red = 4;
		int dark_pink = 5;
		int dark_yellow = 6;
		int dark_white = 7;
		int dark_gray = 8;
		int blue = 9;
		int green = 10;
		int teal = 11;
		int red = 12;
		int pink = 13;
		int yellow = 14;
		int white = 15;
		int DEFAULT = 15;
	};

	class Console {
	public:
		//	Declare Structs
		Colors color;

		//	IO
		FILE* stream_in{};
		FILE* stream_out{};
		FILE* stream_error{};
		char input[32]{};
		char input2[32]{};

		//	ObjectReferences
		HANDLE g_Handle{};
		HWND g_hWnd{};

		//	Class Variables
		bool m_ShowConsole{};
		bool verbose{};
		bool ACTIVE{};

		explicit Console();
		~Console() noexcept = default;
		Console(Console const&) = delete;
		Console(Console&&) = delete;
		Console& operator=(Console const&) = delete;
		Console& operator=(Console&&) = delete;
		
		//	BASIC FUNCTIONS
		void InitializeConsole(const char* ConsoleName);
		void printdbg(const char* Text, int Color = {}, ...);
		void scandbg(const char* Text, ...);
		void LogEvent(std::string TEXT, bool FLAG);
		void DestroyConsole();

		//	ADVANCED FUNCTIONS
		bool writeFile(const char* Path, const char* Text, unsigned int Length, DWORD* out = {});
		bool readFile(const char* Path, char* Text, unsigned int Length, DWORD* out = {});
		void SaveCFG();
		void LoadCFG();

		//	SPECIAL FUNCTIONS
		const wchar_t* Char2WChar(const char* in);
		const char* WChar2Char(const wchar_t* in);
		const wchar_t* String2WChar(std::string in);
		std::string CurrentProcName(CHAR* path);
	};
	inline std::unique_ptr<Console> g_Console;
}