#ifndef ASIMOVPACK_H
#define ASIMOVPACK_H

#include "Package.h"
#include "Sockets.h"

#ifdef ArduinoLinux
#include <wiringSerial.h> // Serial
using namespace cv;
#endif

#ifdef ZBAR
#include <zbar.h>
using namespace zbar;
#define delayGiro 950
#endif

#include <time.h>

#ifdef _WIN32
#include <conio.h>
#endif // _WIN32

#ifdef _MSC_VER
#include "CXBOXController.h"
#endif


#define CLIENT 0
#define SERVER 1

#ifdef _MSC_VER
using namespace System::Drawing;
#endif

class AsimovPack
{
    public:
        AsimovPack();
        virtual ~AsimovPack();

        void FirstTime();
        void Connection();
        void RunCycle();
        void HandShake(char* conta, char* senha);

        //Geral
        int Setup;
        int Porta;
        int TimeoutConection;
        int delayGeral;
		char* Ip;
        Package PacoteDeEnvio, PacoteDeRecebimento;
		void AsimovFull(int Modo);

        //Client
        void getCommands(int tempo);
        char* window_name1;
        char* window_name2;
        #ifdef _MSC_VER
        Bitmap ^iplImageToBitmap(IplImage *image); //Visual Studio
		char comandoVS;
        #endif

        //Server
        void useCommands();
        int Camera;
        int tipoCompressao;
        int rateCompressao;

        //Erros
        bool Connected;

        //Controle
        int PingValue;
        int FPS_Value;

    protected:

    private:
#ifdef _MSC_VER
		CXBOXController* Player1;
#endif

		IplImage* BufferCleaner;

        void PingLag();
        void PongLag();
        void PingFPS();
        void PongFPS();
        void FPS();

        void arduinoEnviarComando( int direcao, int velocidade , int velocidadeFarois , int velocidadeCooler );
        void arduinoStartSerial();
        int idArduino;

        void Close_Free();
        long int before[2];
        long int after[2];
        bool aux;

        Sockets Comunicacao;
        CvCapture* Capturador;
        char *vetorAux;

        #ifdef __GNUC__
        char* get_timed_input(long timeout_mseconds, char* input_buffer,int buffer_size);
        char ComandoGCC[1];
        #endif // __GNUC__

        //comandos
        int vetorComandos[4]; // [0]modo + [1]angulo + [2]velocidade + [3]farol
        bool farol;
        int velocidade;
		int angulo;
};

#endif // ASIMOVPACK_H