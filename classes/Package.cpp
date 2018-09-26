#include "Package.h"

Package::Package()
{
    this->nomeJanela=NULL;

    this->tamanhoTotal=0;
    this->tamanhoMensagem=0;
    this->tamanhoInteiro=0;
    this->tamanhoReal=0;
    this->nImagens=0;

    this->imagensComprimidas.clear();
	this->vetorImagens.clear();
    this->numeroReal=NULL;
    this->numeroInteiro=NULL;
    this->mensagem=NULL;

    defaultConversion.push_back(CV_IMWRITE_JPEG_QUALITY);
    defaultConversion.push_back(10);
    addConversion.push_back(CV_IMWRITE_JPEG_QUALITY);
    addConversion.push_back(10);
}


Package::~Package()
{
    //dtor
}


void Package::setMensagem(char *mensagem, int tamanhoMensagem){
    this->tamanhoMensagem = tamanhoMensagem;
    this->mensagem = mensagem;
}

void Package::setNumeroInteiro(int *numeroInteiro ,int tamanhoInteiro){
    this->tamanhoInteiro = tamanhoInteiro;
    this->numeroInteiro = numeroInteiro;
}

void Package::setNumeroReal(float *numeroReal ,int tamanhoReal){
    this->tamanhoReal = tamanhoReal;
    this->numeroReal = numeroReal;
}

int Package::setImage(IplImage *tpoImage, int Pos){
    if(Pos == -1){
        this->vetorImagens.push_back(tpoImage);
        CompactacaoDefault(Pos);
    }
    else if(Pos < this->vetorImagens.size()){
        this->vetorImagens[Pos] = tpoImage;
        CompactacaoDefault(Pos);
    }
    else{
        return -1;
    }
    return 0;
}

int Package::setImage(IplImage *tpoImage, int Pos, int conversion, int rate){
    addConversion[0]=conversion;
    addConversion[1]=rate;

    if(Pos == -1){
        this->vetorImagens.push_back(tpoImage);
        Compactacao(Pos);
    }
    else if(Pos < this->vetorImagens.size()){
        this->vetorImagens[Pos] = tpoImage;
        Compactacao(Pos);
    }
    else{
        return -1;
    }
    return 0;
}

void Package::clearMensagem(){
    this->mensagem = NULL;
    this->tamanhoMensagem = 0;
}
void Package::clearInteiro(){
    this->numeroInteiro = NULL;
    this->tamanhoInteiro = 0;
}
void Package::clearReal(){
    this->numeroReal = NULL;
    this->tamanhoReal = 0;
}
int Package::clearImagem(int Pos){
    if(Pos < this->vetorImagens.size()){
        this->vetorImagens.erase(this->vetorImagens.begin() + Pos);
        this->imagensComprimidas.erase(this->imagensComprimidas.begin() + Pos);
    }
    else{
        return -1;
    }
    return 0;
}

void Package::freeMensagem(){
    free(this->mensagem);
    this->mensagem = NULL;
    this->tamanhoMensagem = 0;
}
void Package::freeInteiro(){
    free(this->numeroInteiro);
    this->numeroInteiro = NULL;
    this->tamanhoInteiro = 0;
}
void Package::freeReal(){
    free(this->numeroReal);
    this->numeroReal = NULL;
    this->tamanhoReal = 0;
}

int Package::freeImagem(int Pos){
    if(Pos < this->vetorImagens.size()){
        cvReleaseImage(&this->vetorImagens[Pos]);
        this->vetorImagens.erase(this->vetorImagens.begin() + Pos);
        this->imagensComprimidas.erase(this->imagensComprimidas.begin() + Pos);
    }
    else{
        return -1;
    }
    return 0;
}

void Package::freeAllImages(){
    for(int i = 0; i < this->vetorImagens.size(); i++){
        cvReleaseImage(&this->vetorImagens[i]);
    }
    this->vetorImagens.clear();
    this->imagensComprimidas.clear();
}

int* Package::getNumeroInteiro(){
    return this->numeroInteiro;
}

float* Package::getNumeroReal(){
    return this->numeroReal;
}

char* Package::getMensagem(){
    return this->mensagem;
}

IplImage* Package::getImage(int Pos){
    if(Pos < this->vetorImagens.size()){
        return this->vetorImagens[Pos];
    }
    return NULL;
}

int Package::getNumeroDeImagens(){
    return this->vetorImagens.size();
}

void Package::descreverDado()
{
    printf("\n -- Descricao do objeto --\n");
    printf("Tamanho mensagem: %d\n", this->tamanhoMensagem);
    printf("Tamanho inteiro: %d\n", this->tamanhoInteiro);
    printf("Tamanho real: %d\n", this->tamanhoReal);
    printf("Numero de imagens: %d\n", this->vetorImagens.size());

    if(this->tamanhoMensagem != 0){
        printf("\nMensagem: %s", this->mensagem);
    }

    if(this->tamanhoInteiro != 0){
        printf("\nNumero inteiro:");
        for(int i=0; i<this->tamanhoInteiro; i++){
            printf(" %d", this->numeroInteiro[i]);
        }
    }

    if(this->tamanhoReal != 0){
        printf("\nNumero real:");
        for(int i=0; i<this->tamanhoReal; i++){
            printf(" %.3f", this->numeroReal[i]);
        }
    }

    for(int i = 0; i < this->vetorImagens.size(); i++){
        printf("\n\nImagem: %d\n", i+1);
        printf("Largura: %d\n", this->vetorImagens[i]->width);
        printf("Altura: %d\n", this->vetorImagens[i]->height);
        printf("Numero de canais: %d\n", this->vetorImagens[i]->nChannels);
        printf("Tamanho total da imagem: %d\n", this->vetorImagens[i]->imageSize);
        printf("Tamanho total da imagem comprimida: %d\n", this->imagensComprimidas[i].size());
#ifdef _MSC_VER
	char* nomeJanelaN = (char*)malloc(((sizeof(char))*((strlen(this->nomeJanela))+1) + sizeof(int))+1);
#endif

#ifdef __GNUC__
	char* nomeJanelaN = (char*)malloc(((sizeof(char))*((strlen(this->nomeJanela))+1) + sizeof(int)));
#endif
	sprintf(nomeJanelaN, "%s %d", this->nomeJanela, i+1);
    cvShowImage(nomeJanelaN, this->vetorImagens[i]);
    free(nomeJanelaN);
    }
    cout << "\n\n -- FIM --" << endl;
}

char* Package::embrulhaPacote(){
    int tamanhoTotalComprimido = 0;
    for(int i = 0; i < this->imagensComprimidas.size(); i++){
        tamanhoTotalComprimido += this->imagensComprimidas[i].size();
    }
	this->tamanhoTotal = (5 * tamnumeroInteiro) + ((this->imagensComprimidas.size())*tamnumeroInteiro) + this->tamanhoMensagem + ((this->tamanhoInteiro)*tamnumeroInteiro) + ((this->tamanhoReal)*tamnumeroReal) + tamanhoTotalComprimido;
#ifdef _MSC_VER
	char* dados = (char*)malloc(this->tamanhoTotal + 1);
#endif

#ifdef __GNUC__
	char* dados = (char*)malloc(this->tamanhoTotal);
#endif

    memset(dados, '\0', this->tamanhoTotal);
	sprintf(dados, "%10d%10d%10d%10d%10d", this->tamanhoTotal, this->tamanhoMensagem, this->tamanhoInteiro, this->tamanhoReal, this->imagensComprimidas.size());
	char* aux;

#ifdef _MSC_VER
	aux = (char*)malloc(tamnumeroInteiro + 1);
#endif

#ifdef __GNUC__
	aux = (char*)malloc(tamnumeroInteiro);
#endif

    for(int i = 0; i < this->imagensComprimidas.size(); i++){
        memset(aux, '\0', tamnumeroInteiro+1);
        sprintf(aux, "%10d", this->imagensComprimidas[i].size());
        strcat(dados,aux);
    }
    free(aux);


#ifdef _MSC_VER
	aux = (char*)malloc(this->tamanhoMensagem + 1);
#endif

#ifdef __GNUC__
	aux = (char*)malloc(this->tamanhoMensagem);
#endif

    if(this->tamanhoMensagem>0){
        //memset(aux, '\0', this->tamanhoMensagem +1);
        sprintf(aux, "%s",this->mensagem);
        strcat(dados,aux);
    }
    free(aux);

#ifdef _MSC_VER
	aux = (char*)malloc(tamnumeroInteiro + 1);
#endif

#ifdef __GNUC__
	aux = (char*)malloc(tamnumeroInteiro);
#endif

    for(int i=0;i<this->tamanhoInteiro;i++){
        memset(aux, '\0', tamnumeroInteiro+1);
        sprintf(aux, "%10d",this->numeroInteiro[i]);
        strcat(dados,aux);
    }
    free(aux);

#ifdef _MSC_VER
	aux = (char*)malloc(tamnumeroReal + 1);
#endif

#ifdef __GNUC__
	aux = (char*) malloc(tamnumeroReal);
#endif

    for(int i=0;i<this->tamanhoReal;i++){
        memset(aux, '\0', tamnumeroReal+1);
        sprintf(aux, "%15.3f",this->numeroReal[i]);
        strcat(dados,aux);
    }
    free(aux);
    int acumulado = this->tamanhoTotal - tamanhoTotalComprimido;
    for(int i = 0; i < this->imagensComprimidas.size(); i++){
        memcpy(dados + acumulado, this->imagensComprimidas[i].data(), this->imagensComprimidas[i].size());
        acumulado+=this->imagensComprimidas[i].size();
    }
    return dados;
}

void Package::desembrulhaPacote(char* dados){
    int acumulado=0;
	char aux[100];

	memset(aux, '\0', sizeof(aux));
    memcpy(aux, dados, tamnumeroInteiro);
    sscanf(aux, "%d", &(this->tamanhoTotal));
    acumulado+=tamnumeroInteiro;

    memset(aux, '\0', sizeof(aux));
    memcpy(aux, dados + tamnumeroInteiro, tamnumeroInteiro);
    sscanf(aux, "%d", &(this->tamanhoMensagem));
    acumulado+=tamnumeroInteiro;

    memset(aux, '\0', sizeof(aux));
    memcpy(aux, dados + 2*tamnumeroInteiro, tamnumeroInteiro);
    sscanf(aux, "%d", &(this->tamanhoInteiro));
    acumulado+=tamnumeroInteiro;

    memset(aux, '\0', sizeof(aux));
    memcpy(aux, dados + 3*tamnumeroInteiro, tamnumeroInteiro);
    sscanf(aux, "%d", &(this->tamanhoReal));
    acumulado+=tamnumeroInteiro;

    memset(aux, '\0', sizeof(aux));
    memcpy(aux, dados + 4*tamnumeroInteiro, tamnumeroInteiro);
    sscanf(aux, "%d", &(this->nImagens));
    acumulado+=tamnumeroInteiro;

#ifdef _MSC_VER
	int *tamanhoImagens = (int*)malloc((sizeof(int)*this->nImagens) + 1);
#endif

#ifdef __GNUC__
	int *tamanhoImagens = (int*)malloc((sizeof(int)*this->nImagens));
#endif

    for(int i=0; i<this->nImagens;i++){
        memset(aux, '\0', sizeof(aux));
        memcpy(aux, dados + acumulado , tamnumeroInteiro);
        sscanf(aux, "%d", &(tamanhoImagens[i]));
        acumulado+=tamnumeroInteiro;
    }

#ifdef _MSC_VER
    this->mensagem = (char*)malloc(this->tamanhoMensagem + 1);
#endif

#ifdef __GNUC__
	this->mensagem = (char*)malloc(this->tamanhoMensagem);
#endif

    memset(this->mensagem, '\0', this->tamanhoMensagem +1);
    memcpy(this->mensagem, dados + acumulado , this->tamanhoMensagem);
    acumulado+=this->tamanhoMensagem;

#ifdef _MSC_VER
	this->numeroInteiro = (int*)malloc((sizeof(int)*this->tamanhoInteiro) + 1);
#endif

#ifdef __GNUC__
	this->numeroInteiro = (int*)malloc((sizeof(int)*this->tamanhoInteiro));
#endif

    for(int i=0; i<this->tamanhoInteiro;i++){
        memset(aux, '\0', sizeof(aux));
        memcpy(aux, dados + acumulado , tamnumeroInteiro);
        sscanf(aux, "%d", &(this->numeroInteiro[i]));
        acumulado+=tamnumeroInteiro;
    }

#ifdef _MSC_VER
	this->numeroReal = (float*)malloc((sizeof(float)*this->tamanhoReal) + 1);
#endif

#ifdef __GNUC__
	this->numeroReal = (float*)malloc((sizeof(float)*this->tamanhoReal));
#endif

    for(int i=0; i<this->tamanhoReal;i++){
        memset(aux, '\0', sizeof(aux));
        memcpy(aux, dados + acumulado , tamnumeroReal);
        sscanf(aux, "%f", &(this->numeroReal[i]));
        acumulado+=tamnumeroReal;
    }
	for(int i=0; i<this->nImagens;i++){
        this->imagensComprimidas.resize(this->imagensComprimidas.size() +1);
		this->imagensComprimidas.back().resize(tamanhoImagens[i]);
		memcpy(this->imagensComprimidas.back().data(), dados + acumulado, tamanhoImagens[i]);
		Descompactacao();
		acumulado+=tamanhoImagens[i];
	}
	free(tamanhoImagens);
}

void Package::setNomeJanela(char *nomeJanela){
    if(this->nomeJanela != NULL){
        free(this->nomeJanela);
    }

#ifdef _MSC_VER
	this->nomeJanela = (char*)malloc(strlen(nomeJanela));
#endif

#ifdef __GNUC__
	this->nomeJanela =(char*) malloc(strlen(nomeJanela));
#endif

    strcpy(this->nomeJanela, nomeJanela);
}

int Package::getTamanhoTotal(){
	return this->tamanhoTotal;
}

int Package::getTamanhoReal(){
	return this->tamanhoReal;
}

int Package::getTamanhoInteiro(){
	return this->tamanhoInteiro;
}

int Package::getTamanhoMensagem(){
	return this->tamanhoMensagem;
}

void Package::CompactacaoDefault(int numImagem){
    if(numImagem == -1){
        this->imagensComprimidas.resize(this->imagensComprimidas.size() +1);
        cv::imencode(".jpg", cv::cvarrToMat(this->vetorImagens.back()), this->imagensComprimidas.back(), defaultConversion);
    }
    else if(numImagem < this->imagensComprimidas.size()){
        this->imagensComprimidas[numImagem].clear();
        cv::imencode(".jpg", cv::cvarrToMat(this->vetorImagens[numImagem]), this->imagensComprimidas[numImagem], defaultConversion);
    }
}

void Package::Compactacao(int numImagem){
    if(numImagem == -1){
        this->imagensComprimidas.resize(this->imagensComprimidas.size() +1);
        cv::imencode(".jpg", cv::cvarrToMat(this->vetorImagens.back()), this->imagensComprimidas.back(), addConversion);
    }
    else if(numImagem < this->imagensComprimidas.size()){
        this->imagensComprimidas[numImagem].clear();
        cv::imencode(".jpg", cv::cvarrToMat(this->vetorImagens[numImagem]), this->imagensComprimidas[numImagem], addConversion);
    }
}

void Package::Descompactacao(){
    cv::Mat imagemDescomprimidaAux = cv::imdecode(cv::Mat(this->imagensComprimidas.back()), 1);
    IplImage aux = imagemDescomprimidaAux;
    this->vetorImagens.push_back(cvCloneImage(&aux));
}
