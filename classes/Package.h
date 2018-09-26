#ifndef Package_H
#define Package_H

#include "DefinicoesImportantes.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>
#include <stdio.h>

#define tamnumeroInteiro 10
#define tamnumeroReal 15

using namespace std;

class Package
{
    public:
        Package();
        virtual ~Package();
        void descreverDado();
        char* embrulhaPacote();
        void desembrulhaPacote(char* dados);

        void setNumeroInteiro(int *numeroInteiro,int tamanhoInteiro);
        int* getNumeroInteiro();
        void setNumeroReal(float *numeroReal,int tamanhoReal);
        float* getNumeroReal();
        void setMensagem(char *mensagem, int tamanhoMensagem);
        char* getMensagem();
        int setImage(IplImage* tpoImage, int Pos);
        int setImage(IplImage* tpoImage, int Pos, int conversion, int rate);
        IplImage* getImage(int Pos);
        int getNumeroDeImagens();

        void clearMensagem();
        void clearInteiro();
        void clearReal();
        int clearImagem(int Pos);

        void freeMensagem();
        void freeInteiro();
        void freeReal();
        int freeImagem(int Pos);
        void freeAllImages();

		int getTamanhoTotal();
		int getTamanhoMensagem();
		int getTamanhoInteiro();
		int getTamanhoReal();

        void setNomeJanela(char *nomeJanela);

		void CompactacaoDefault(int numImagem);
		void Compactacao(int numImagem);
		void Descompactacao();

    protected:
    private:
        char *nomeJanela;

        int tamanhoTotal;
        int tamanhoMensagem;
        int tamanhoInteiro;
        int tamanhoReal;
        int nImagens;

		vector<IplImage*> vetorImagens;
		vector<int> defaultConversion;
		vector<int> receiveConversion;
		vector<int> addConversion;
		vector< vector<unsigned char> > imagensComprimidas;
        float *numeroReal;// reservar 8 bytes
        int *numeroInteiro;// reservar 5 bytes
        char *mensagem;// reservar 255 bytes

};

#endif // Package_H
