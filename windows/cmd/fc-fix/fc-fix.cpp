//---------------------------------------------------------------------------

#include <windows.h>
#include <stdio.h>
#include <shellapi.h>
#include <shlobj.h>
#include <string>
#include <string.h>
#include <iostream>
#include <fstream>


using namespace std;



#pragma hdrstop

//---------------------------------------------------------------------------


/*
        To have fontconfig work right , it requires some config files located at \\etc\\fonts\\ of
        where fc functions are called .We also need to insert windows font directory to fc config file
*/

int fix_fontconfig()
{
        char B[1024];
        string FontFolder="";
        string fcConfigFile="";
		string BB="";
		//retrieve font directory
		::SHGetSpecialFolderPath( NULL,B , CSIDL_FONTS , 0 );
                //check if directory already exists

		//retrieve current directory
		TCHAR curWorkingDir[2024];
		GetCurrentDirectory(2024,curWorkingDir);
		//set fc config file
		fcConfigFile.append(curWorkingDir);
		fcConfigFile.append("\\etc\\fonts\\fonts.conf");
		cout << fcConfigFile;
		
		ifstream file (fcConfigFile.c_str(),ios::in);
		if (file)
		{
			while (!file.eof())
			{
				BB.push_back(file.get());
			}
			file.close();
	//		 cout << BB;
	//		 cout << "\n" <<"position of found text:" <<BB.find("#WINDOWSFONTDIR#",0 );
			size_t a=BB.find("#WINDOWSFONTDIR#",0 );
			if (a !=string::npos)
				BB.replace ( a , strlen(B),B );
			std::ofstream out(fcConfigFile.c_str());
			out<< BB <<'\n';
			out.close();
		}
		return 0;
}


int add_to_path(string path)
{

	string Path="";
	string graphvizfolder="";
	HKEY hKey = 0;
	char buf[1024] = {0};
	DWORD dwType = 0;
	DWORD dwBufSize = sizeof(buf);

	const char* subkey = "SYSTEM\\CurrentControlSet\\Control\\Session Manager\\Environment";
	if( RegOpenKeyEx(HKEY_LOCAL_MACHINE,subkey,0, KEY_ALL_ACCESS,&hKey) == ERROR_SUCCESS)
	{
		dwType = REG_EXPAND_SZ;
		if( RegQueryValueEx(hKey,"Path",0, &dwType, (BYTE*)buf, &dwBufSize) == ERROR_SUCCESS)
		{
			TCHAR curWorkingDir[2024];
			GetCurrentDirectory(2024,curWorkingDir);
//			cout << "key value is '" << buf << "'\n";
			graphvizfolder.append(curWorkingDir);
			graphvizfolder.append("\\bin");
			Path.append(buf);
			if (Path.find(graphvizfolder.c_str(),0 )== string::npos)
			{
				Path.append(";");
				Path.append(curWorkingDir);
				Path.append("\\bin;");
				RegSetValueEx (hKey, "Path", 0L, dwType, (BYTE*)Path.c_str(), (DWORD)strlen(Path.c_str())+ 1 );			
                //[this block is to inform system about path change, so to avoid restart
                DWORD dwReturnValue;
                SendMessageTimeout(HWND_BROADCAST, WM_SETTINGCHANGE,
                        0, (LPARAM) "Environment", SMTO_ABORTIFHUNG, 5000, &dwReturnValue);

			}
		}
		RegCloseKey(hKey);
	}
	return 0;
}






int main(int argc, char* argv[])
{
	/*this little utility is called right after graphviz is intalled by msi package
		its job is to finalize installation by modifing few things.
	*/
	add_to_path("");
	fix_fontconfig();
	return 0;
}
//---------------------------------------------------------------------------
