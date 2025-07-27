///////////////////////////////////////////////////////////////
// Created: 21.01.2025
// Author: ChatGPT, NS_Deathman
// Input realization
///////////////////////////////////////////////////////////////
#include "stdafx.h"
///////////////////////////////////////////////////////////////
#include "Input.h"
///////////////////////////////////////////////////////////////
CInput::CInput()
{
	std::cout << "Initializing input... \n";

	m_KeyBoardStates = SDL_GetKeyboardState(NULL);

	std::fill(std::begin(m_bKeyPressed), std::end(m_bKeyPressed), false);
	std::fill(std::begin(m_bGamepadButtonPressed), std::end(m_bGamepadButtonPressed), false);

	if(SDL_NumJoysticks())
		m_GameController = SDL_GameControllerOpen(0);

	m_bNeedHandleCursorWithGameController = true;

	GetCursorPos(&m_ptLastCursorPosition);
}

void CInput::HandleCursorWithGameController()
{
	if (m_GameController && m_bNeedHandleCursorWithGameController)
	{
		float LeftStickX = 0.0f;
		float LeftStickY = 0.0f;

		GamepadGetLeftStick(LeftStickX, LeftStickY);

		GetCursorPos(&m_ptLastCursorPosition);

		// Adjust the cursor position based on the right stick input
		m_ptLastCursorPosition.x += static_cast<LONG>(LeftStickX * 10.0f); // Adjust scaling factor as needed
		m_ptLastCursorPosition.y += static_cast<LONG>(LeftStickY * 10.0f); // Subtract y-axis because screen coordinates typically have 0 at the top

		SetCursorPos(m_ptLastCursorPosition.x, m_ptLastCursorPosition.y);

		if (GamepadButtonHolded(SDL_CONTROLLER_BUTTON_A))
			mouse_event(MOUSEEVENTF_LEFTDOWN, m_ptLastCursorPosition.x, m_ptLastCursorPosition.y, 0, 0);

		if (GamepadButtonReleased(SDL_CONTROLLER_BUTTON_A))
			mouse_event(MOUSEEVENTF_LEFTUP, m_ptLastCursorPosition.x, m_ptLastCursorPosition.y, 0, 0);
	}
}

void CInput::OnFrame(SDL_Event& Event)
{
	// Update the keyboard state
	m_KeyBoardStates = SDL_GetKeyboardState(NULL);

	if (SDL_NumJoysticks() && !m_GameController)
	{
		m_GameController = SDL_GameControllerOpen(Event.cdevice.which);
		if (m_GameController)
			std::cout << "New game controller device opened successfuly \n";
	}
	else if (SDL_NumJoysticks() == 0 && m_GameController)
	{
		SDL_GameControllerClose(m_GameController);
		m_GameController = nullptr; // Set to null after closing
		std::cout << "Game controller device closed successfully \n";
	}

	while (SDL_PollEvent(&Event))
	{
		switch (Event.type)
		{
		case SDL_KEYDOWN:
			m_bKeyPressed[Event.key.keysym.scancode] = true;
			break;
		case SDL_KEYUP:
			m_bKeyPressed[Event.key.keysym.scancode] = false;
			break;
		}
	}

	HandleCursorWithGameController();
}

bool CInput::KeyPressed(int key)
{
	// Check if the key is currently pressed
	if (m_KeyBoardStates[key])
	{
		// If it wasn't already marked as pressed, it means this is the first press
		if (!m_bKeyPressed[key])
		{
			// Update the pressed state to true
			m_bKeyPressed[key] = true;
			// Return true indicating the key was just pressed
			return true;
		}
	}
	else
	{
		// If the key is not pressed, reset its state for next frame
		m_bKeyPressed[key] = false;
	}

	// Return false indicating the key was not just pressed
	return false;
}

bool CInput::KeyHolded(int key)
{
	// Return true if the key is currently being held down
	return m_KeyBoardStates[key];
}

bool CInput::GamepadButtonPressed(int button)
{
	// Check if the gamepad button is pressed
	if (m_GameController)
	{
		if (SDL_GameControllerGetButton(m_GameController, static_cast<SDL_GameControllerButton>(button)) == SDL_PRESSED)
		{
			// If it wasn't already marked as pressed, it means this is the first press
			if (!m_bGamepadButtonPressed[button])
			{
				// Update the pressed state to true
				m_bGamepadButtonPressed[button] = true;
				// Return true indicating the key was just pressed
				return true;
			}
		}
		else
		{
			// If the key is not pressed, reset its state for next frame
			m_bGamepadButtonPressed[button] = false;
		}
	}
	return false;
}

bool CInput::GamepadButtonHolded(int button)
{
	// Return true if the gamepad button is being held down
	return m_GameController && SDL_GameControllerGetButton(m_GameController, static_cast<SDL_GameControllerButton>(button)) == SDL_PRESSED;
}

bool CInput::GamepadButtonReleased(int button)
{
	// Return true if the gamepad button is being held down
	return m_GameController && SDL_GameControllerGetButton(m_GameController, static_cast<SDL_GameControllerButton>(button)) == SDL_RELEASED;
}

// New method to get left stick position
void CInput::GamepadGetLeftStick(float& x, float& y)
{
	if (m_GameController)
	{
		x = SDL_GameControllerGetAxis(m_GameController, SDL_CONTROLLER_AXIS_LEFTX) / 32767.0f; // Normalize to [-1, 1]
		y = SDL_GameControllerGetAxis(m_GameController, SDL_CONTROLLER_AXIS_LEFTY) / 32767.0f; // Normalize to [-1, 1]
	}
}

// New method to get right stick position
void CInput::GamepadGetRightStick(float& x, float& y)
{
	if (m_GameController)
	{
		x = SDL_GameControllerGetAxis(m_GameController, SDL_CONTROLLER_AXIS_RIGHTX) / 32767.0f; // Normalize to [-1, 1]
		y = SDL_GameControllerGetAxis(m_GameController, SDL_CONTROLLER_AXIS_RIGHTY) / 32767.0f; // Normalize to [-1, 1]
	}
}
///////////////////////////////////////////////////////////////
