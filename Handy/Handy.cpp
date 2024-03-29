// ConsoleApplication1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Manus.h"
#include <iostream>
#include <chrono>
#include <ctime>
#include <thread>



void print_hand_data(manus_hand_t handData) {
	for (int i = 0; i < 10; i++) {
		std::cout << handData.raw.finger_sensor[i];
		if (i < 9)
			std::cout << ", ";
	}
	std::cout << std::endl;
}

void print_timestamp() {
	std::chrono::time_point<std::chrono::system_clock> time_now = std::chrono::system_clock::now();
	std::time_t time_now_t = std::chrono::system_clock::to_time_t(time_now);
	std::cout << std::ctime(&time_now_t) << std::endl;
}

bool check_device_connection(manus_session_t & manus_session, device_type_t device) {
	int num_attempts = 1000;
	int i = 0;
	bool ret = false;
	while (i < num_attempts) {
		if (ManusIsConnected(manus_session, device)) {
			ret = true;
			break;
		}
		i++;
	}
	if (ret)
		std::cout << "device detected!" << std::endl;
	else {
		std::cout << "device not detected. Do you want to continue (Y/n)?" << std::endl;
		char continue_app;
		std::cin >> continue_app;
		if (continue_app == 'Y' || continue_app == 'y') {
			std::cout << "Ok. Continuing without device." << std::endl;
			ret = true;
		}
		else {
			std::cout << "Got it. Ciao." << std::endl;
			ret = false;
		}
	}
	return ret;
}

int main()
{

	std::cout << "-------------------------------------------------------------------------" << std::endl;
	std::cout << "Welcome to Handy application 0.1" << std::endl;
	std::cout << "You must calibrate your Manus VR gloves before running this app." << std::endl;
	std::cout << "By Paulo Ferreira (p dot a dot ferreira at cs dot bham dot ac dot uk)" << std::endl;
	std::cout << "-------------------------------------------------------------------------" << std::endl;

	uint32_t num_dongles_to_use = 1;

	// Initializing session of MANUS-VR
	manus_session_t session = nullptr;
	uint32_t dongle_ids[1];
	manus_hand_t leftHandData{}, rightHandData{};


	manus_ret_t manus_ret = ManusInit(&session);

	if (session != nullptr) {
		std::cout << "Manus session initiated. " << ManusErrorString(manus_ret) << std::endl;
		// check dongle connection
		/*
		uint32_t num_dongles_in_use = ManusGetDongleIDs(session, dongle_ids, num_dongles_to_use);
		if (num_dongles_in_use == 0) {
		std::cout << dongle_ids[0] << std::endl;
		std::cout << "No dongles detected. Check if calibration tool can detect the dongle." << std::endl;
		ManusExit(session);
		return 1;
		}
		else {
		std::cout << "Found " << num_dongles_in_use << " dongle(s); using first one from the list: " << std::endl;
		if (dongle_ids == NULL)
		std::cout << "NULL pointer at list of dongles!" << std::endl;
		for (int i = 0; i < num_dongles_in_use; i++) {
		std::cout << dongle_ids[i] << std::endl;
		}
		uint32_t main_dongle_id = dongle_ids[0];
		}
		*/
		// Check glove connections
		std::cout << "Left glove ";
		if (!check_device_connection(session, GLOVE_LEFT)) {
			ManusExit(session);
			return 0;
		}
		std::cout << "Right glove ";
		if (!check_device_connection(session, GLOVE_RIGHT)) {
			ManusExit(session);
			return 0;
		}
		// output sensor data
		std::cout << "BEGIN_OUTPUT" << std::endl;
		int i = 0;
		int sleep_time = 10;
		//std::cout << "Hand, thumb1, thumb2, index1, index2, middle1, middle2, ring1, ring2, little1, little2" << std::endl;
		print_timestamp();
		clock_t start = clock();
		while (true) {
			std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time));
			ManusGetHand(session, GLOVE_LEFT, &leftHandData);
			ManusGetHand(session, GLOVE_RIGHT, &rightHandData);
			clock_t finish = clock();
			double elapsed = (double)(finish - start) / CLOCKS_PER_SEC;
			std::cout << elapsed << std::endl;
			std::cout << "LEFT_HAND, ";
			print_hand_data(leftHandData);
			std::cout << "RIGHT_HAND, ";
			print_hand_data(rightHandData);
			i++;
		}
		std::cout << "END_OUTPUT" << std::endl;
		ManusExit(session);
	}
	else {
		std::cout << "Could not initiate Manus session. " << ManusErrorString(manus_ret) << std::endl;
	}
	return 0;
}

