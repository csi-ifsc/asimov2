#include "AsimovPack.h"

AsimovPack::AsimovPack(){
	this->window_name1 = "Imagem Enviada";
	this->window_name2 = "Imagem Recebida";
	this->Camera = 0;
	this->TimeoutConection = 10;
	this->delayGeral = 500;
	this->Connected = false;
	this->tipoCompressao = CV_IMWRITE_JPEG_QUALITY;
	this->rateCompressao = 10;
	this->aux = false;

	this->vetorComandos[0] = 0;
	this->vetorComandos[1] = 0; //angulo
	this->vetorComandos[2] = 0; //velocidade
	this->vetorComandos[3] = 0;
	this->velocidade = 180;
	this->angulo = 0;
	this->farol = false;

#ifdef _MSC_VER
	this->comandoVS = ' ';
#endif // _MSC_VER
	//ctor
}

AsimovPack::~AsimovPack(){
	//dtor
}

void AsimovPack::AsimovFull(int Modo){
	if (Modo == CLIENT){

		this->Setup = CLIENT;
		this->Porta = 2000;
		this->TimeoutConection = 3;

		this->FirstTime();
		while (1){
			this->Connection();
			this->HandShake("conta", "senha");
			while (this->Connected){
				this->getCommands(1);
				this->RunCycle();
				cvWaitKey(1);
			}
		}
	}
	else if (Modo == SERVER){

		this->Setup = SERVER;
		this->Porta = 2000;
		this->Camera = -1;
		this->TimeoutConection = 3;
		this->rateCompressao = 50;

		this->FirstTime();
		while (1){
			this->Connection();
			this->HandShake("conta", "senha");
			while (this->Connected){
				this->RunCycle();
				this->useCommands();
				cvWaitKey(1);
			}
		}
	}
}

void AsimovPack::arduinoEnviarComando( int direcao, int velocidade , int velocidadeFarois , int velocidadeCooler ){
    #ifdef ArduinoLinux
    /* Formato dos dados recebidos: "d<direção>v<velocidade>x<servo X>y<servo Y>f<faróis>c<cooler>"
     *
     * Velocidade: Varia de 0 a 255
     * Direção: Varia de 0 a 360, 0 sendo a direita, 90 frente, 180 esquerda, 270 trás. -1 é aceito como código para parar.
     * Servo X: Não implementado
     * Servo Y: Não implementado
     * Faróis: Varia de 0 a 1
     * Cooler: Varia de 0 a 1
     *
     */

     string comandoEnviado = "";
     comandoEnviado += "v" + to_string(velocidade)
                     + "d" + to_string(direcao)
                     + "f" + to_string(velocidadeFarois)
                     + "c" + to_string(velocidadeCooler);
    //int fddd = serialPrintf(this->idArduino, "lalalalalal");
    printf("Arduino enviado: %s\n", comandoEnviado.c_str());
    serialPuts(this->idArduino, comandoEnviado.c_str());
    serialFlush(this->idArduino);

    #endif // ArduinoLinux
}

void AsimovPack::arduinoStartSerial(){
    #ifdef ArduinoLinux
    this->idArduino = serialOpen("/dev/ttyACM0", 9600);
    if (this->idArduino == -1)
        this->idArduino = serialOpen("/dev/ttyACM1", 9600);
    if (this->idArduino == -1)
        this->idArduino = serialOpen("/dev/ttyACM2", 9600);
    if (this->idArduino == -1)
        this->idArduino = serialOpen("/dev/ttyACM3", 9600);
    arduinoEnviarComando(0, 0, 0, 0);
    #endif // ArduinoLinux
}

void AsimovPack::FirstTime(){
	switch (this->Setup){

	case CLIENT:

		this->PacoteDeEnvio.setNomeJanela(this->window_name1);
		this->PacoteDeRecebimento.setNomeJanela(this->window_name2);
		this->PacoteDeEnvio.setNumeroInteiro(this->vetorComandos, 4);
#ifdef _WIN32
		this->Comunicacao.TestVersion();
#endif
#ifdef _MSC_VER
		Player1 = new CXBOXController(1);
#endif
		break;

	case SERVER:

        this->arduinoStartSerial();
		this->PacoteDeEnvio.setNomeJanela(this->window_name1);
		this->PacoteDeRecebimento.setNomeJanela(this->window_name2);
		this->Capturador = cvCreateCameraCapture(this->Camera);
		while (this->Capturador == NULL){
			cout << "Camera desconectada." << endl;
			this->Capturador = cvCreateCameraCapture(this->Camera);
			cvWaitKey(this->delayGeral);
		}
		cout << "Camera Conectada!" << endl;
		this->PacoteDeEnvio.setImage(cvQueryFrame(Capturador), -1);
		break;
	}
}
void AsimovPack::Connection(){
	switch (this->Setup){
		int Teste;

	case CLIENT:
		this->Comunicacao.DefinePort(this->Porta);
#ifdef _WIN32
		this->Comunicacao.DefineIp(this->Ip);
		Teste = this->Comunicacao.Connect();
		while (Teste != 0){
			Teste = this->Comunicacao.Connect();
			cvWaitKey(this->delayGeral);
		}
#endif
		this->Comunicacao.Timeout(this->TimeoutConection);
		this->Connected = true;
		break;

	case SERVER:
#ifndef _WIN32
		this->Comunicacao.DefinePort(this->Porta);
		this->Comunicacao.CreateSocket();
		Teste = this->Comunicacao.ListenConnection();
		while (Teste != 0){
			Teste = this->Comunicacao.ListenConnection();
			cvWaitKey(this->delayGeral);
		}
		Teste = this->Comunicacao.TestConnection();
		while (Teste != 0){
			if (this->Comunicacao.testIp(this->Ip)){
				printf("IpCorreto!\n");
				this->arduinoEnviarComando(0, 0, 0, 1);
			}
			else{
				this->arduinoEnviarComando(0, 0, 0, 0);
			}
			Teste = this->Comunicacao.TestConnection();
			cvWaitKey(this->delayGeral);
		}
#endif
		this->Comunicacao.Timeout(this->TimeoutConection);
		this->Connected = true;
		break;
	}
}
void AsimovPack::HandShake(char* conta, char* senha){
}

#ifdef _WIN32
void AsimovPack::PingLag(){
	this->before[0] = GetTickCount();
}

void AsimovPack::PongLag(){
	this->after[0] = GetTickCount();
	this->PingValue = this->after[0] - this->before[0];
}

void AsimovPack::PingFPS(){
	this->before[1] = GetTickCount();
}

void AsimovPack::PongFPS(){
	this->after[1] = GetTickCount();
	this->FPS_Value = 1000 / (this->after[1] - this->before[1]);
}

#endif

void AsimovPack::Close_Free(){
	this->Connected = false;
	free(this->vetorAux);
	this->Comunicacao.CloseConection();
}

void AsimovPack::FPS(){
#ifdef _WIN32
	if (this->aux == false){
		this->PingFPS();
		this->aux = true;
	}
	else{
		this->PongFPS();
		this->aux = false;
	}
#endif // _WIN32
}

void AsimovPack::getCommands(int tempo){
#ifdef __GNUC__
	if (this->get_timed_input(10, this->ComandoGCC, tempo) == NULL){
		this->vetorComandos[1] = 0;
		this->vetorComandos[2] = 0;
	}
	else{

		switch (this->ComandoGCC[0]){
		case 'w':
			this->angulo = 90;
			this->vetorComandos[2] = this->velocidade;
			break;

		case 'a':
			this->angulo = 180;
			this->vetorComandos[2] = this->velocidade;
			break;

		case 's':
			this->angulo = 270;
			this->vetorComandos[2] = this->velocidade;
			break;

		case 'd':
			this->angulo = 0;
			this->vetorComandos[2] = this->velocidade;
			break;

		case 'q':
			this->angulo = 0;
			this->vetorComandos[2] = 0;
			if (this->velocidade > 0){
				this->velocidade--;
			}
			break;

		case 'e':
			this->angulo = 0;
			this->vetorComandos[2] = 0;
			if (this->velocidade < 255){
				this->velocidade++;
			}
			break;

		case 'f':
			this->angulo = 0;
			this->vetorComandos[2] = 0;
			this->farol = !this->farol;
			if (this->farol){
				this->vetorComandos[3] = 1;
			}
			else{
				this->vetorComandos[3] = 0;
			}
			break;

		case 'm':
			this->angulo = 0;
			this->vetorComandos[2] = 0;
			scanf("%d", &this->vetorComandos[0]);
			fflush(stdin);
			break;

		}
		this->vetorComandos[1] = this->angulo;
	}
#endif // __GNUC__

#ifdef _MSC_VER

	if (Player1->IsConnected()){
		/*
		if (Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_X) //Quadrado
		{

		}

		if (Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_B) //Bola
		{

		}

		if (Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_Y) //Triangulo
		{

		}*/

		if (Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_A) //X
		{
			this->farol = !this->farol;
		}

		this->velocidade = (int)((Player1->GetState().Gamepad.bRightTrigger) - (Player1->GetState().Gamepad.bLeftTrigger));

		this->angulo = (int)((Player1->GetState().Gamepad.sThumbLX) / 128);
		if (this->velocidade >= 0){
			angulo = ((-angulo + 255) / 2.834);
		}
		else if (this->velocidade < 0){
			angulo = (((angulo + 255) / 2.834) + 180);
			this->velocidade = -velocidade;
		}

		this->vetorComandos[2] = this->velocidade;
	}

	else{
		switch (this->comandoVS){

		case 'q':
			if (this->velocidade > 0){
				this->velocidade--;
			}
			break;

		case 'e':
			if (this->velocidade < 255){
				this->velocidade++;
			}
			break;

		case 'f':
			this->farol = !this->farol;
			cvWaitKey(50);
			break;

		case 'w':
			this->angulo = 90;
			this->vetorComandos[2] = this->velocidade;
			break;

		case 's':
			this->angulo = 270;
			this->vetorComandos[2] = this->velocidade;
			break;

		case 'a':
			this->angulo = 180;
			this->vetorComandos[2] = this->velocidade;
			break;

		case 'd':
			this->angulo = 0;
			this->vetorComandos[2] = this->velocidade;
			break;

		default:
			this->vetorComandos[2] = 0;
			this->angulo = 0;
			break;
		}
	}
	// sempre acontece
	if (this->farol){
		this->vetorComandos[3] = 1;
	}
	else{
		this->vetorComandos[3] = 0;
	}

	this->vetorComandos[1] = this->angulo;

#endif

}

void AsimovPack::useCommands(){
	printf("Modo: %d \nAngulo: %d \nVelocidade: %d \nFarol: %d\n", this->PacoteDeRecebimento.getNumeroInteiro()[0], this->PacoteDeRecebimento.getNumeroInteiro()[1], this->PacoteDeRecebimento.getNumeroInteiro()[2], this->PacoteDeRecebimento.getNumeroInteiro()[3]);
	if (this->PacoteDeRecebimento.getNumeroInteiro()[0] == 0){
		arduinoEnviarComando(this->PacoteDeRecebimento.getNumeroInteiro()[1], this->PacoteDeRecebimento.getNumeroInteiro()[2], this->PacoteDeRecebimento.getNumeroInteiro()[3], 1);
	}
#ifdef ZBAR
	else if(this->PacoteDeRecebimento.getNumeroInteiro()[0] == 1){        //ZBAR
        for(int i = 0; i < 5;i++){
            this->BufferCleaner = cvQueryFrame(Capturador);
		}
        ImageScanner scanner;

        //le qualquer simbolo
        //scanner.set_config(ZBAR_NONE, ZBAR_CFG_ENABLE, 1);
        //le somente QRCode
        scanner.set_config(ZBAR_QRCODE, ZBAR_CFG_ENABLE, 1);

        //for(int i = 0; i < 4;i++){
		//this->cachorrofofinho = cvQueryFrame(Capturador);
		//}

        Mat imagemCol(this->BufferCleaner);
        Mat imagemCinza;
        cvtColor(imagemCol,imagemCinza,CV_RGB2GRAY);

        int width = imagemCinza.cols;
        int height = imagemCinza.rows;
        uchar *raw = (uchar *)imagemCinza.data;
        Image image(width, height, "Y800", raw, width * height);
        int n = scanner.scan(image);
        cout << "N " << n << endl;
        //Point pontos[4] = {Point(0, 0),Point(0 , height),Point(width, 0),Point(width, height)};

        for(Image::SymbolIterator symbol = image.symbol_begin();symbol != image.symbol_end();++symbol) {
            vector<Point> vp;
            cout << "decoded " << symbol->get_type_name()
            << " symbol \"" << symbol->get_data() << '"' <<" "<< endl;/*
            int n = symbol->get_location_size();
            for(int i=0;i<n;i++){
                vp.push_back(Point(symbol->get_location_x(i),symbol->get_location_y(i)));
                *//*
                if(symbol->get_location_x(i) > pontos[0].x && symbol->get_location_y(i) > pontos[0].y){
                    pontos[0] = Point(symbol->get_location_x(i),symbol->get_location_y(i));
                }
                if(symbol->get_location_x(i) > pontos[1].x && symbol->get_location_y(i) < pontos[1].y){
                    pontos[1] = Point(symbol->get_location_x(i),symbol->get_location_y(i));
                }
                if(symbol->get_location_x(i) < pontos[2].x && symbol->get_location_y(i) > pontos[2].y){
                    pontos[2] = Point(symbol->get_location_x(i),symbol->get_location_y(i));
                }
                if(symbol->get_location_x(i) < pontos[3].x && symbol->get_location_y(i) < pontos[3].y){
                    pontos[3] = Point(symbol->get_location_x(i),symbol->get_location_y(i));
                }
                *//*
            }
            RotatedRect r = minAreaRect(vp);
            Point2f pts[4];
            r.points(pts);
            for(int i=0;i<4;i++){
                cvLine(this->PacoteDeEnvio.getImage(0),pts[i],pts[(i+1)%4],Scalar(255,0,0),3);
                //line(imgout,pontos[i],pontos[(i+1)%4],Scalar(0,255,0),3);
            }
*/
            //cout<<"Angle: "<<r.angle<<endl;
            string a = symbol->get_data();
            char b = a[0];
            switch (b){
                case 'T':
                    this->arduinoEnviarComando(270, 100, this->PacoteDeRecebimento.getNumeroInteiro()[3], 1);
                    break;
                case 'F':
                    this->arduinoEnviarComando(90, 100, this->PacoteDeRecebimento.getNumeroInteiro()[3], 1);
                    break;
                case 'D':
                    this->arduinoEnviarComando(0, 180, this->PacoteDeRecebimento.getNumeroInteiro()[3], 1);
                    cvWaitKey(delayGiro);
                    this->arduinoEnviarComando(90, 100, this->PacoteDeRecebimento.getNumeroInteiro()[3], 1);
                    break;
                case 'E':
                    this->arduinoEnviarComando(180, 180, this->PacoteDeRecebimento.getNumeroInteiro()[3], 1);
                    cvWaitKey(delayGiro);
                    arduinoEnviarComando(90, 100, this->PacoteDeRecebimento.getNumeroInteiro()[3], 1);
                    break;
                case 'P':
                    this->arduinoEnviarComando(0, 0, this->PacoteDeRecebimento.getNumeroInteiro()[3], 1);
                    break;
            }
            continue;
        }
        image.set_data(NULL, 0);
	}
#endif
}

#ifdef _MSC_VER
Bitmap ^iplImageToBitmap(IplImage *image) //Visual Studio
{
	Bitmap ^bitmap;
	if (image->nChannels == 1)
	{
		bitmap = gcnew Bitmap(image->width, image->height, System::Drawing::Imaging::PixelFormat::Format8bppIndexed);

		System::Drawing::Imaging::ColorPalette  ^palette = bitmap->Palette;
		for (int i = 0; i<256; i++)
			palette->Entries[i] = Color::FromArgb(i, i, i);
		bitmap->Palette = palette;
	}
	else
		bitmap = gcnew Bitmap(image->width, image->height, System::Drawing::Imaging::PixelFormat::Format24bppRgb);

	System::Drawing::Imaging::BitmapData ^data = bitmap->LockBits(System::Drawing::Rectangle(0, 0, bitmap->Width, bitmap->Height), System::Drawing::Imaging::ImageLockMode::ReadWrite, bitmap->PixelFormat);
	memcpy(data->Scan0.ToPointer(), image->imageData, image->imageSize);
	bitmap->UnlockBits(data);

	return bitmap;
}
#endif // _MSC_VER

#ifdef __GNUC__
char* AsimovPack::get_timed_input(long timeout_mseconds, char* input_buffer, int buffer_size){
#ifdef _WIN32
	clock_t start, now;

	//time_t start, now;
	int nChars = 0;
	int chr = 0;
	//const int VK_RETURN = 13;
	while (nChars < buffer_size && chr != VK_RETURN)
	{
		// start timer
		//time(&start);
		start = clock();
		while (!_kbhit())
		{
			//time(&now);
			now = clock();
			if ((now - start) >= timeout_mseconds)
			{
				// timeout
				//
				input_buffer[nChars] = 0;
				return NULL;
			}
		}
		chr = getche();
		input_buffer[nChars++] = chr;
	}
	// null terminate input buffer
	input_buffer[nChars] = 0;
	return input_buffer;
#endif // _WIN32
}
#endif

void AsimovPack::RunCycle(){
	static bool Last = false;

	switch (this->Setup){
		int Erro;

	case CLIENT:

		this->vetorAux = this->PacoteDeEnvio.embrulhaPacote();
		Erro = this->Comunicacao.Send(this->vetorAux, this->PacoteDeEnvio.getTamanhoTotal());
		if (Erro == -1){
			Close_Free();
			break;
		}

		free(this->vetorAux);

		this->vetorAux = this->Comunicacao.Receive();
		if (this->vetorAux == NULL){
			Close_Free();
			break;
		}

		this->PacoteDeRecebimento.freeImagem(0);

		this->PacoteDeRecebimento.desembrulhaPacote(this->vetorAux);
		free(this->vetorAux);

#ifndef _MSC_VER
		system("cls");
		cout << "\nFPS: " << this->FPS_Value << endl;
		this->PacoteDeEnvio.descreverDado();
		this->PacoteDeRecebimento.descreverDado();
#endif

		this->FPS();

		break;

	case SERVER:

		this->vetorAux = this->Comunicacao.Receive();
		if (this->vetorAux == NULL){
			Close_Free();
			break;
		}

		this->PacoteDeRecebimento.freeInteiro();
		this->PacoteDeRecebimento.freeReal();
		this->PacoteDeRecebimento.freeMensagem();

		this->PacoteDeRecebimento.desembrulhaPacote(this->vetorAux);
		free(this->vetorAux);

		if (this->PacoteDeRecebimento.getNumeroInteiro()[0] == 0){
			for (int i = 0; i < 5; i++){
				this->BufferCleaner = cvQueryFrame(Capturador);
			}

			if (Last){
				this->PacoteDeEnvio.setImage(cvQueryFrame(Capturador), -1, this->tipoCompressao, this->rateCompressao);
			}
			else{
				this->PacoteDeEnvio.setImage(cvQueryFrame(Capturador), 0, this->tipoCompressao, this->rateCompressao);
			}
			Last = false;
		}
		else{
			this->BufferCleaner = cvQueryFrame(Capturador);
			this->PacoteDeEnvio.clearImagem(0);
			Last = true;
		}

		this->vetorAux = this->PacoteDeEnvio.embrulhaPacote();

		Erro = this->Comunicacao.Send(this->vetorAux, this->PacoteDeEnvio.getTamanhoTotal());
		if (Erro == -1){
			Close_Free();
			break;
		}
		free(this->vetorAux);

		system("clear");
		this->PacoteDeEnvio.descreverDado();
		this->PacoteDeRecebimento.descreverDado();

		break;
	}
}
