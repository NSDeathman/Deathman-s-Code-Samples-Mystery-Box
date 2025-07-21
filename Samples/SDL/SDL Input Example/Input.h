///////////////////////////////////////////////////////////////
// Created: 21.01.2025
// Author: ChatGPT, NS_Deathman
// Input realization
///////////////////////////////////////////////////////////////
#pragma once
///////////////////////////////////////////////////////////////
class CInput
{
  private:
	const Uint8* m_KeyBoardStates;

	// Array to track key pressed states
	bool m_bKeyPressed[SDL_NUM_SCANCODES];
	bool m_bGamepadButtonPressed[SDL_CONTROLLER_BUTTON_MAX];

	SDL_GameController* m_GameController; // Pointer to the game controller

	bool m_bNeedHandleCursorWithGameController;

	POINT m_ptLastCursorPosition;

  public:
	CInput();
	~CInput() = default;

	void HandleCursorWithGameController();

	void OnFrame(SDL_Event& Event);

	bool KeyPressed(int key);
	bool KeyHolded(int key);

	bool GamepadButtonPressed(int button);
	bool GamepadButtonHolded(int button);
	bool GamepadButtonReleased(int button);

	void GamepadGetLeftStick(float& x, float& y);
	void GamepadGetRightStick(float& x, float& y);

	void SetNeedUpdateCursorWithGameController(bool flag)
	{
		m_bNeedHandleCursorWithGameController = flag;
	}
};
///////////////////////////////////////////////////////////////
