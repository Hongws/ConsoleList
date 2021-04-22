// ConsoleApplication1.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#pragma warning(disable: 4996)

#include <afx.h>

#include <string>
using namespace std;


#pragma comment(lib, "rpcrt4.lib")
#pragma comment (lib, "setupapi.lib")

#include <windows.h>
#include <devguid.h>    // for GUID_DEVCLASS_CDROM etc
#include <setupapi.h>
#include <cfgmgr32.h>   // for MAX_DEVICE_ID_LEN, CM_Get_Parent and CM_Get_Device_ID
#define INITGUID
#include <tchar.h>
#include <stdio.h>


// include DEVPKEY_Device_BusReportedDeviceDesc from WinDDK\7600.16385.1\inc\api\devpropdef.h
#ifdef DEFINE_DEVPROPKEY
#undef DEFINE_DEVPROPKEY
#endif
#ifdef INITGUID
#define DEFINE_DEVPROPKEY(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8, pid) EXTERN_C const DEVPROPKEY DECLSPEC_SELECTANY name = { { l, w1, w2, { b1, b2,  b3,  b4,  b5,  b6,  b7,  b8 } }, pid }
#else
#define DEFINE_DEVPROPKEY(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8, pid) EXTERN_C const DEVPROPKEY name
#endif // INITGUID

// include DEVPKEY_Device_BusReportedDeviceDesc from WinDDK\7600.16385.1\inc\api\devpkey.h
DEFINE_DEVPROPKEY(DEVPKEY_Device_BusReportedDeviceDesc, 0x540b947e, 0x8b40, 0x45bc, 0xa8, 0xa2, 0x6a, 0x0b, 0x89, 0x4c, 0xbd, 0xa2, 4);     // DEVPROP_TYPE_STRING
DEFINE_DEVPROPKEY(DEVPKEY_Device_ContainerId, 0x8c7ed206, 0x3f8a, 0x4827, 0xb3, 0xab, 0xae, 0x9e, 0x1f, 0xae, 0xfc, 0x6c, 2);     // DEVPROP_TYPE_GUID
DEFINE_DEVPROPKEY(DEVPKEY_Device_FriendlyName, 0xa45c254e, 0xdf1c, 0x4efd, 0x80, 0x20, 0x67, 0xd1, 0x46, 0xa8, 0x50, 0xe0, 14);    // DEVPROP_TYPE_STRING
DEFINE_DEVPROPKEY(DEVPKEY_DeviceDisplay_Category, 0x78c34fc8, 0x104a, 0x4aca, 0x9e, 0xa4, 0x52, 0x4d, 0x52, 0x99, 0x6e, 0x57, 0x5a);  // DEVPROP_TYPE_STRING_LIST
DEFINE_DEVPROPKEY(DEVPKEY_Device_LocationInfo, 0xa45c254e, 0xdf1c, 0x4efd, 0x80, 0x20, 0x67, 0xd1, 0x46, 0xa8, 0x50, 0xe0, 15);    // DEVPROP_TYPE_STRING
DEFINE_DEVPROPKEY(DEVPKEY_Device_Manufacturer, 0xa45c254e, 0xdf1c, 0x4efd, 0x80, 0x20, 0x67, 0xd1, 0x46, 0xa8, 0x50, 0xe0, 13);    // DEVPROP_TYPE_STRING
DEFINE_DEVPROPKEY(DEVPKEY_Device_SecuritySDS, 0xa45c254e, 0xdf1c, 0x4efd, 0x80, 0x20, 0x67, 0xd1, 0x46, 0xa8, 0x50, 0xe0, 26);    // DEVPROP_TYPE_SECURITY_DESCRIPTOR_STRING
DEFINE_DEVPROPKEY(DEVPKEY_Device_BusRelations, 0x4340a6c5, 0x93fa, 0x4706, 0x97, 0x2c, 0x7b, 0x64, 0x80, 0x08, 0xa5, 0xa7, 7);     // DEVPROP_TYPE_STRING_LIST

DEFINE_DEVPROPKEY(DEVPKEY_Device_DriverProvider, 0xa8b865dd, 0x2e3d, 0x4094, 0xad, 0x97, 0xe5, 0x93, 0xa7, 0xc, 0x75, 0xd6, 9);      // DEVPROP_TYPE_STRING
DEFINE_DEVPROPKEY(DEVPKEY_Device_DriverPropPageProvider, 0xa8b865dd, 0x2e3d, 0x4094, 0xad, 0x97, 0xe5, 0x93, 0xa7, 0xc, 0x75, 0xd6, 10);     // DEVPROP_TYPE_STRING

#define ARRAY_SIZE(arr)     (sizeof(arr)/sizeof(arr[0]))

typedef BOOL(WINAPI *FN_SetupDiGetDevicePropertyW)(
	__in       HDEVINFO DeviceInfoSet,
	__in       PSP_DEVINFO_DATA DeviceInfoData,
	__in       const DEVPROPKEY *PropertyKey,
	__out      DEVPROPTYPE *PropertyType,
	__out_opt  PBYTE PropertyBuffer,
	__in       DWORD PropertyBufferSize,
	__out_opt  PDWORD RequiredSize,
	__in       DWORD Flags
	);

struct Device { //设备
	/*USB*/
	char USB_PORT[100];//USB端口
	char Serial_Number[100];//序列号
	char Parent_Class_ID[200];//父类ID
	char bus_relations[1000];//总线关系	
	int type;//0普通 1AEE

	//AndFile wpd_and_file;
};


//向cmd发送指令
CString ExecuteCmd(CString str)
{
	SECURITY_ATTRIBUTES sa;
	HANDLE hRead, hWrite;

	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = TRUE;
	if (!CreatePipe(&hRead, &hWrite, &sa, 0))
	{
		MessageBox(nullptr, "Error on CreatePipe()!", "demo", MB_OK);
		return "";
	}
	STARTUPINFO si = { sizeof(si) };
	PROCESS_INFORMATION pi;
	si.hStdError = hWrite;
	si.hStdOutput = hWrite;
	si.wShowWindow = SW_HIDE;
	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	if (!CreateProcess(NULL, str.GetBuffer(), NULL, NULL, TRUE, NULL, NULL, NULL, &si, &pi))
	{
		MessageBox(nullptr, "Error on CreateProcess()!", "demo", MB_OK);
		return "";
	}
	CloseHandle(hWrite);
	str.ReleaseBuffer();
	char buffer[4096];
	memset(buffer, 0, 4096);
	CString output;
	DWORD byteRead;
	while (true)
	{
		if (ReadFile(hRead, buffer, 4095, &byteRead, NULL) == NULL)
		{
			break;
		}
		int iRet = GetLastError();
		printf("ExecuteCmd iRet:%d\n", iRet);
		output += buffer;
	}
	return output;
}
#include <shlwapi.h>

string strpath1 = "";

string USBPATH(string serial_number)//挂载
{
	CString asd = ExecuteCmd("cmd /c,mountvol /r");
	string path = "";
	HKEY hKey;
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SYSTEM\\MountedDevices", 0, KEY_READ, &hKey) == ERROR_SUCCESS)
	{
		TCHAR    achKey[1000];
		DWORD    cbName;
		DWORD    retCode = 0;
		DWORD    retNum = 0;
		DWORD    cchValue = 1000;
		while (retCode == 0)
		{
			cbName = 1000;
			retCode = RegEnumValue(hKey, retNum, achKey, &cbName, NULL, NULL, NULL, NULL);
			retNum++;
			TCHAR Temp[1000];
			DWORD len(1000);
			DWORD dwType = REG_BINARY;
			string str;

			if (!RegQueryValueEx(hKey, achKey, NULL, &dwType, (LPBYTE)Temp, &len))
			{
				for (int i = 0; i < len; i++)
				{
					str += Temp[i];
					i++;
				}
			}

			string strTemp = achKey;
			strpath1 = achKey;

			printf("USBPATH str:%s\n", str.data());
			printf("USBPATH strTemp:%s\n", strTemp.data());

			if (strTemp.find("Volume") != -1)
			{
				if (str.find("CdRom") != -1)//CD 盘跳出
					continue;

				int ok = str.find("#" + serial_number);
				if (ok > 0 && serial_number != "")
				{
					strTemp = strTemp.substr(strTemp.find("Volume"));
					path = "c:\\usb\\";
					if (!PathIsDirectory(path.data()))
						CreateDirectory(path.data(), NULL);
					path = "c:\\usb\\" + strTemp;

					RemoveDirectory(path.data());
					if (!PathIsDirectory(path.data()))
					{
						CreateDirectory(path.data(), NULL);
						string cmd;
						cmd = "cmd /c,mountvol c:\\usb\\" + strTemp + " " + "\\\\?\\" + strTemp + "\\";
						printf("cmd	:%s\n", cmd.data());
						asd = ExecuteCmd(cmd.data());
						printf("asd	:%s\n", asd);
					}
				}
			}
		}
	}

	return path;
}




int main()
{
	HDEVINFO hDevInfo = NULL;

	TCHAR GUIDString[50] = _T("36FC9E60-C465-11CF-8056-444553540000");
	GUID guid;
	UuidFromString((unsigned char *)GUIDString, &guid);


	hDevInfo = SetupDiGetClassDevs(&guid, NULL, NULL, DIGCF_PRESENT);//SetupDiGetClassDevs 
	DWORD i;
	SP_DEVINFO_DATA DeviceInfoData;
	ZeroMemory(&DeviceInfoData, sizeof(SP_DEVINFO_DATA));
	DeviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
	//g_log("---------------------start----------");
	for (i = 0; SetupDiEnumDeviceInfo(hDevInfo, i, &DeviceInfoData); ++i)
	{
		Device device_data;
		memset(&device_data, 0, sizeof(Device));
		//获取device_data.Parent_Class_ID
		SetupDiGetDeviceRegistryProperty(hDevInfo, &DeviceInfoData, SPDRP_HARDWAREID, NULL, (PBYTE)device_data.Parent_Class_ID, sizeof(device_data.Parent_Class_ID) - 1, NULL);//硬件ID 

		//获取device_data.USB_PORT  SPDRP_BASE_CONTAINERID  SPDRP_LOCATION_PATHS
		//if (0 == g_nGetUsbIdea)
		SetupDiGetDeviceRegistryProperty(hDevInfo, &DeviceInfoData, SPDRP_LOCATION_INFORMATION, NULL, (PBYTE)device_data.USB_PORT, sizeof(device_data.USB_PORT) - 1, NULL);
		//else if (1 == g_nGetUsbIdea)
		//	SetupDiGetDeviceRegistryProperty(hDevInfo, &DeviceInfoData, SPDRP_LOCATION_PATHS, NULL, (PBYTE)device_data.USB_PORT, sizeof(device_data.USB_PORT) - 1, NULL);
		//else
		//{
		//	char szTemp[200] = "";
		//	SetupDiGetDeviceRegistryProperty(hDevInfo, &DeviceInfoData, SPDRP_BASE_CONTAINERID, NULL, (PBYTE)szTemp, sizeof(szTemp) - 1, NULL);
		//	int j = 0;
		//	for (int i = 0; i < 200; i++)
		//	{
		//		if (szTemp[i] != '\0')
		//			device_data.USB_PORT[j++] = szTemp[i];
		//		else if (szTemp[i + 1] == '\0')
		//			break;
		//		//if (szTemp[i + 2] != '\n' && szTemp[i+3] != '\n')
		//		//	device_data.USB_PORT[i] = szTemp[i];
		//		//else
		//		//	break;
		//	}
		//}
		SetupDiGetDeviceInstanceId(hDevInfo, &DeviceInfoData, device_data.Serial_Number, LINE_LEN, 0);//序列号 


		string Parent_Class_ID = device_data.Parent_Class_ID;

		printf("di %d :Parent_Class_ID:%s\n", i, Parent_Class_ID.data());
		if (Parent_Class_ID.find("USB\\VID_070A&PID_4026") != -1
			);
		else
		{
			continue;
		}

		/*获取总线关系*/
		DEVPROPTYPE ulPropertyType;
		WCHAR szBuffer[4096];
		DWORD dwSize;
		FN_SetupDiGetDevicePropertyW fn_SetupDiGetDevicePropertyW = (FN_SetupDiGetDevicePropertyW)
			GetProcAddress(GetModuleHandle(TEXT("Setupapi.dll")), "SetupDiGetDevicePropertyW");
		memset(szBuffer, 0, sizeof(szBuffer));
		if (fn_SetupDiGetDevicePropertyW && fn_SetupDiGetDevicePropertyW(hDevInfo, &DeviceInfoData, &DEVPKEY_Device_BusRelations,
			&ulPropertyType, (BYTE*)szBuffer, sizeof(szBuffer), &dwSize, 0));
		sprintf(device_data.bus_relations, "%ws", szBuffer);
		CString disk = device_data.bus_relations;
		disk.MakeUpper();

		printf("device_data.bus_relations:%s\n", device_data.bus_relations);

		if (disk.Find("DISK") == -1)
		{
			continue;
		}
		string serial_number = device_data.bus_relations;
		serial_number = serial_number.substr(serial_number.find_last_of("\\") + 1);
		serial_number = serial_number.substr(0, serial_number.find_last_of("&"));
		printf("serial_number:%s\n", serial_number.data());


		USBPATH(serial_number);
		MessageBox(nullptr, serial_number.data(), "finally", MB_OK);
	}

	system("pause");

	return 0;
}


