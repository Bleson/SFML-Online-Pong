namespace Pong{
	enum ServerMessage {
		PlayerNumber,
		PlayerConnected,
		PlayerDisconnected,
		Snapshot,
		RoundEnd,
		GameEnd,
		PlayerMovement
	};

	enum ClientMessage {
		Quit,
		Move,
	};
}