#include "Game.h"

#ifdef MC_DIST
int WINAPI wWinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR lpCmdLine,
	_In_ int nShowCmd
) {
	UNREFERENCED_PARAMETER(hInstance);
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	UNREFERENCED_PARAMETER(nShowCmd);
#else
int main()
{
#endif

	Game game;
	if (!game.Init())
	{
		std::cout << "Game could not be initialized!" << std::endl;
		return -1;
	}
	game.Run();

	game.Release();

	return 0;
}
