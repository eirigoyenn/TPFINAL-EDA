#pragma once
#include "GenericFSM.h"
#include "GUIEventGenerator.h"
#include <vector>
#include <time.h>
#include <stdlib.h>

#define TX(x) (static_cast<void (genericFSM::* )(genericEvent *)>(&FSM::x)) //casteo a funcion, por visual


enum implStates : stateTypes {InitState, ShwDashboard, Look4Veci, ShwNodos, ShwSelBlock , ShwError , SHwGENESIS, Multi};

using namespace std;

class FSM : public genericFSM
{
public:
	FSM() : genericFSM(&fsmTable[0][0], 8, 10, InitState), state4Graphic(INITSTATE_G) {};
	~FSM();

	unsigned int state4Graphic;
	std::vector<SPVNode*>* getSPVArrayPTR(void);
	std::vector<FullNode*>* getFULLArrayPTR(void);
	Blockchain& getBchain(void);

private:
	const fsmCell fsmTable[8][10] = {

				//EVENTOS:		          Crear Nodo							 Crear Conexion									Mostrar Nodos						Buscar Vecinos								EnviarMsj									Error										    Back2Dashboard											No event								BlockSelected										FINISHED GENESIS NETWORK               
		//ESTADOS																										/*Start app mode en init state*/		/*Start genesis mode en init state*/
		/* Init State */		{{InitState,TX(RutaDefault)},			{InitState,TX(CrearConexion_r_acc)},			{ShwDashboard,TX(Start_app_r_acc)},	  {SHwGENESIS,TX(Start_genesis_r_acc)},			{ShwDashboard,TX(RutaDefault)},				{ShwError,TX(ErrorEncontrado_r_acc)} ,				{ShwDashboard,TX(VolverADashboard_r_acc)} ,			{InitState,TX(RutaDefaultInitState)} ,	{ShwSelBlock,TX(BlockSelected_r_acc)}     ,       {InitState,TX(RutaDefault)}  },

		/*Shw Dashboard*/		{{ShwDashboard,TX(CrearNodo_r_acc)},     {ShwDashboard,TX(CrearConexion_r_acc)},		{ShwNodos,TX(ShwNodos_r_acc)},			{Look4Veci,TX(BuscarVecinos_r_acc)},		{ShwDashboard,TX(RutaDefault)},				{ShwError,TX(ErrorEncontrado_r_acc)} ,				{ShwDashboard,TX(VolverADashboard_r_acc)} ,			{ShwDashboard,TX(MultiiPerform)} ,		{ShwSelBlock,TX(BlockSelected_r_acc)}     ,       {ShwDashboard,TX(RutaDefault)}    },

		/*  Look4Veci  */		{{Look4Veci,TX(RutaDefault)},			{Look4Veci,TX(RutaDefault)},					{Look4Veci,TX(RutaDefault)},			{Look4Veci,TX(RutaDefault)},				{ShwDashboard,TX(EnviarMensaje_r_acc)},     {ShwError,TX(ErrorEncontrado_r_acc)},				{ShwDashboard,TX(VolverADashboard_r_acc)} ,			{Look4Veci, TX(MultiiPerform) } ,		{ShwDashboard,TX(RutaDefault) }           ,       {Look4Veci,TX(RutaDefault)}   },

		/*  ShwNodos   */		{{ShwNodos,TX(RutaDefault)},			{ShwNodos,TX(RutaDefault)},						{ShwNodos,TX(RutaDefault)},				{ShwNodos,TX(RutaDefault)},					{ShwDashboard,TX(RutaDefault)},				{ShwError,TX(ErrorEncontrado_r_acc)},				{ShwDashboard,TX(VolverADashboard_r_acc)}	,		{ShwNodos, TX(MultiiPerform)} ,			{ShwDashboard,TX(RutaDefault)}            ,       {ShwNodos,TX(RutaDefault)}    },
		 
		/*  ShwSelectedBlock */	{{ShwNodos,TX(RutaDefault)},			{ShwNodos,TX(RutaDefault)},						{ShwNodos,TX(RutaDefault)},				{ShwNodos,TX(RutaDefault)},					{ShwDashboard,TX(RutaDefault)},				{ShwError,TX(ErrorEncontrado_r_acc)},				{ShwDashboard,TX(VolverADashboard_r_acc)}	,		{ShwSelBlock, TX(MultiiPerform)} ,		{ShwDashboard,TX(RutaDefault)}		      , 	  {ShwNodos,TX(RutaDefault)}    },
		
		/*   ShwError  */		{{ShwError,TX(RutaDefault)},			{ShwError,TX(RutaDefault)},						{ShwError,TX(RutaDefault)},				{ShwError,TX(RutaDefault)},					{ShwDashboard,TX(RutaDefault)},				{ShwError,TX(ErrorEncontrado_r_acc)},				{ShwDashboard,TX(VolverADashboard_r_acc)} , 		{ShwError,TX(MultiiPerform)} ,			{ShwDashboard,TX(RutaDefault)}            ,		  {ShwError,TX(RutaDefault)}    },

		/* SHwGENESIS */		{{SHwGENESIS,TX(RutaDefault)},			{SHwGENESIS,TX(RutaDefault)},					{SHwGENESIS,TX(RutaDefault)},			{SHwGENESIS,TX(RutaDefault)},				{SHwGENESIS,TX(RutaDefault)},				{SHwGENESIS,TX(RutaDefault)},						{SHwGENESIS,TX(RutaDefault)} , 						{SHwGENESIS,TX(cycle_each_r_acc)} ,		{SHwGENESIS,TX(RutaDefault)}          ,			  {ShwDashboard,TX(finish_r_acc)}  },
			
		/* SHwGENESIS */		{{Multi,TX(RutaDefault)},				{Multi,TX(RutaDefault)},						{Multi,TX(RutaDefault)},				{Multi,TX(RutaDefault)},					{Multi,TX(RutaDefault)},					{Multi,TX(RutaDefault)},							{Multi,TX(RutaDefault)} , 							{Multi,TX(cycle_each_r_acc)} ,			{Multi,TX(RutaDefault)}          ,				 {Multi,TX(finish_r_acc)}  }
	};

	//The action routines for the FSM
	//These actions should not generate fsmEvents

	/* FUNCIONES */
	void Add2JSONfile(bool isFullNode, int puerto);

	//void selectRandomFullNode(int i);

	std::ofstream BulletinFileR_ACC;

	boost::asio::io_context io_context;

	void RutaDefault(genericEvent* ev);

	void RutaDefaultInitState(genericEvent* ev);

	void BuscarVecinos_r_acc(genericEvent* ev);

	void EnviarMensaje_r_acc(genericEvent* ev);

	void CrearNodo_r_acc(genericEvent* ev);

	void finish_r_acc(genericEvent* ev);

	void cycle_each_r_acc(genericEvent* ev);

	void CrearConexion_r_acc(genericEvent* ev);
	
	void VolverADashboard_r_acc(genericEvent* ev);
	
	void ErrorEncontrado_r_acc(genericEvent* ev);
	
	void ShwNodos_r_acc(genericEvent* ev);
	
	void BlockSelected_r_acc(genericEvent* ev);
	
	void MultiiPerform(genericEvent* ev);
	
	void Start_genesis_r_acc(genericEvent* ev);
	
	void Start_app_r_acc(genericEvent* ev);
	
	unsigned int getIndex(unsigned int senderID, nodeTypes nodeType);
	
	unsigned int getneighbourIDfromPort(unsigned int neighbourPORT, nodeTypes nodetype);
	
	void parsingRED(json);
	
	std::vector<SPVNode*> spvArray;
	
	std::vector<FullNode*> fullArray;
	std::vector<unsigned int> portsArray;

	unsigned int makeRandomTime(void);

//	bool isNetworkReady();

	json InputRED;
	Blockchain Bchain;

};