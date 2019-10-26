#pragma once

namespace Define
{
	const int WelcomePacket = 1;
	const int UpdateCountPlayer = 2;
	const int LetStart = 3;
	const int TankPacket = 4;
	const int WorldStatePacket = 5;
	const int InputPacket = 6;
	const int RequestName = 7;
	const int JoinRoon = 8;
	const int ExitGame = 9;
	const int CreateRoom = 10;
	const int GameOver = 11;
	const int OutRoom = 12;
	const int SyncTime = 13;

	const int bitofTypePacket = 4;
	const int bitofID = 11;
	const int bitofLocation = 12;
	const int NBit_RoomID = 2; // dao động từ 0 - 3
}
