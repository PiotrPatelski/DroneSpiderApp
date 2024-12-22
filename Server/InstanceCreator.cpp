#include "InstanceCreator.hpp"
#include "ListeningConnection.hpp"
#include "AdminInstance.hpp"
#include <iostream>
#include "DroneConnectionStub.hpp"

InstanceCreator::InstanceCreator() {

}

InstanceCreator::~InstanceCreator() {

}

std::unique_ptr<ListeningConnection> InstanceCreator::createListeningConnection() {
	std::cout << "InstanceCreator:: create ListeningConnection\n";
	return std::make_unique<ListeningConnection>();
}

std::unique_ptr<AdminInstance> InstanceCreator::createAdminInstance(ListeningConnection& acceptedConnection) {
	std::cout << "InstanceCreator:: create AdminInstance\n";
	auto worker = std::make_unique<AdminInstance>(
		std::make_unique<AdminConnectionProxy>(
			acceptedConnection.releaseTcpPipe(),
			acceptedConnection.releaseUdpPipe()),
		acceptedConnection.getConnectedClientId());
	return std::move(worker);
}

std::unique_ptr<DroneInstance> InstanceCreator::createDroneInstance(ListeningConnection& acceptedConnection) {
	std::cout << "InstanceCreator:: create DroneInstance\n";
	auto worker = std::make_unique<DroneInstance>(
		std::make_unique<DroneConnectionProxy>(
			acceptedConnection.releaseTcpPipe(),
			acceptedConnection.releaseUdpPipe()),
		acceptedConnection.getConnectedClientId());
	return std::move(worker);
}

//TODO REMOVE STUB CODE
std::unique_ptr<DroneInstance> InstanceCreator::createDroneStub(ListeningConnection& acceptedConnection) {
	std::cout << "InstanceCreator:: create DroneStub\n";
	auto worker = std::make_unique<DroneInstance>(
		std::make_unique<DroneConnectionStub>(
			acceptedConnection.releaseTcpPipe(),
			acceptedConnection.releaseUdpPipe()),
		99);
	return std::move(worker);
}
//
