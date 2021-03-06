#include "biblioteca.h"

BIBLIOTECA::BIBLIOTECA(BANCODENOTAS* ba, BEEP* be) :
    ARVORE<MUSICA*>(){

    minhaMusica = NULL;
    banco = ba;
    beep = be;
    bibdir = "bib/";
    bibfile = "bib/biblioteca.bib";
    recarregar();
}

BANCODENOTAS* BIBLIOTECA::getBanco(){
    return banco;
}

BEEP* BIBLIOTECA::getBeep(){
    return beep;
}

MUSICA* BIBLIOTECA::getMinhaMusica(){
    return minhaMusica;
}

void BIBLIOTECA::setMinhaMusica(MUSICA* mus){
    minhaMusica = mus;
}

void BIBLIOTECA::recarregar(){

    destruir(raiz);

    // musica usada no teste
    MUSICA* mus = new MUSICA(beep,"Teste");
    NOTA n(banco);
    bool okn;
    n.setNota("0do", okn);
    mus->adicionar(banco->getNota(n), okn);
    n.setNota("0mi", okn);
    mus->adicionar(banco->getNota(n), okn);
    n.setNota("0sol", okn);
    mus->adicionar(banco->getNota(n), okn);
    n.setNota("1do", okn);
    mus->adicionar(banco->getNota(n), okn);
    n.setNota("0sol", okn);
    mus->adicionar(banco->getNota(n), okn);
    n.setNota("0mi", okn);
    mus->adicionar(banco->getNota(n), okn);
    n.setNota("0do", okn);
    mus->adicionar(banco->getNota(n), okn);
    insere(mus);


    // Abrindo arquivo biblioteca.bib

    char* bibfilec = new char[bibfile.length()+1];
    strcpy( bibfilec, bibfile.c_str() );

    int conf = GetFileAttributes(TEXT(bibfilec));
    if(conf == -1 || conf == FILE_ATTRIBUTE_DIRECTORY){
        cout << "Arquivo nao existe"<<endl;
        return;
    }

    ifstream ifs(bibfilec);
	string str;
	bool ok;

    // Copiando endere�os das m�sicas
	ifs >> str;
    while(!ifs.eof()){
        abrir(str);
        moveMinhaMusicaPBib();
        bibfiles.adicionar(str, ok);
        ifs >> str;
    }

    delete bibfilec;
}

void BIBLIOTECA::moveMinhaMusicaPBib(){
    if(minhaMusica != NULL){
        minhaMusica->setNome(normalizaNome(minhaMusica->getNome()));
        while( estaNaArvore(minhaMusica) ){
            string novoNome;
            cout << "Este nome ja existe na biblioteca: " << minhaMusica->getNome() << endl;
            cout << "Digite outro nome: ";
            cin >> novoNome;
            minhaMusica->setNome(normalizaNome(novoNome));
        }

        insere(minhaMusica);
        minhaMusica = NULL;
    }
}

void BIBLIOTECA::tocar(string s, bool& ok){
    NO2<MUSICA*>* noMus = NULL;
    bool encontrado = false;
    busca(s, raiz, noMus, encontrado);
    if(encontrado){
        ok = true;
        noMus->getInfo()->tocar();
    } else {
        ok = false;
    }
}

MUSICA* BIBLIOTECA::getMusica(string s, bool &ok){
    NO2<MUSICA*>* noMus = NULL;
    bool encontrado = false;
    busca(s, raiz, noMus, encontrado);
    if(encontrado){
        ok = true;
        return noMus->getInfo();
    } else {
        ok = false;
        return NULL;
    }
}

string BIBLIOTECA::normalizaNome(string s){
    for(unsigned int i=0; i<s.length(); i++){
        if(s[i] == ' ')
            s[i] = '_';
    }
    return s;
}

bool BIBLIOTECA::salvar(){
    // Pedindo o nome do arquivo
    string nomeP;
    cout << "Salvando: entre com nome do arquivo: ";
    cin >> nomeP;
    nomeP = normalizaNome(nomeP);
    while(bibfiles.estaNaLista(nomeP)){
        cout << "Este arquivo ja existe: " << nomeP << endl;
        cout << "Digite outro nome de arquivo: ";
        cin >> nomeP;
        nomeP = normalizaNome(nomeP);
        if(nomeP == "")
            return false;
    }

    // Verificando se o nome da m�sica j� existe
    string novoNome;
    while(estaNaArvore(minhaMusica)){
        cout << "Este nome de musica ja existe: " << minhaMusica->getNome() << endl;
        cout << "Digite outro nome: ";
        cin >> novoNome;
        normalizaNome(novoNome);
        minhaMusica->setNome(novoNome);
    }

    // Adicionando a biblioteca
    bool ok;
    bibfiles.adicionar(nomeP, ok);
    salvarBib();

    nomeP = bibdir + nomeP;
    char* arqNome = new char[nomeP.length()+1];
    strcpy( arqNome, nomeP.c_str() );

    ofstream ofs(arqNome);

    // Salvando nome
    ofs << "nome," << normalizaNome(minhaMusica->getNome()) << endl;

    // Salvando notas
    NO2<NOTA*> *noPtr;
    noPtr = minhaMusica->getPtrHeader()->getDir();
    while(noPtr!=minhaMusica->getPtrHeader()){
        ofs << noPtr->getInfo()->getNome() << "," << noPtr->getInfo()->getTempo() << endl;
        noPtr = noPtr->getDir();
    }

    insere(minhaMusica);
    minhaMusica = NULL;
    delete arqNome;

    return true;
}

void BIBLIOTECA::salvarBib(){

    if(!bibfiles.vazia()){
        char* bibfilec = new char[bibfile.length()+1];
        strcpy( bibfilec, bibfile.c_str() );
        ofstream ofsbib(bibfilec);

        NO2<string>* ptrMus = bibfiles.getPtrHeader()->getDir();
        while(ptrMus != bibfiles.getPtrHeader()){
            ofsbib << ptrMus->getInfo() << endl;
            ptrMus = ptrMus->getDir();
        }

        delete bibfilec;
    }

}


bool BIBLIOTECA::abrir(string nomeP){
    nomeP = bibdir + nomeP;

    char* arqNome = new char[nomeP.length()+1];
    strcpy( arqNome, nomeP.c_str() );

    // VERIFICANDO EXISTENCIA DO ARQUIVO
    int conf = GetFileAttributes(TEXT(arqNome));

    if(conf == -1 || conf == FILE_ATTRIBUTE_DIRECTORY){
        cout << "Arquivo nao existe " << arqNome <<endl;
        return false;
    }

    string entrada;

    // VERIFICANDO OVERWRITE
    if(minhaMusica != NULL){
        delete minhaMusica;
        minhaMusica = new MUSICA(beep);
    } else {
        minhaMusica = new MUSICA(beep);
    }

    // ABRINDO IFS
	ifstream ifs(arqNome);
	string str,strTempo;
	float tempo;
    bool okn, okt;

	NOTA minhaNota(banco);

    // LENDO DO IFS
	ifs >> str;
	string arg1 = "";
	string arg2 = "";
    while(!ifs.eof()){
            if(str!=""){
            arg1 = str.substr(0,str.find(','));
            arg2 = str.substr(str.find(',')+1);
            if(arg1 == "nome"){
                minhaMusica->setNome(normalizaNome(arg2));
            } else {

                tempo = ::atof(arg2.c_str());
                minhaNota.setNota(arg1,tempo,okn,okt);
                if(! minhaMusica->insereAEsquerda(banco->getNota(minhaNota),*minhaMusica->getPtrHeader()))
                    cout << "Erro, Nota nao reconhecida: " << str << endl;
            }
        }
        ifs >> str;
    }

    delete arqNome;
    return true;
}

void BIBLIOTECA::novaMusica(){
    if(minhaMusica != NULL){
        delete minhaMusica;
        minhaMusica = NULL;
    }
    minhaMusica = new MUSICA(beep);
}

bool BIBLIOTECA::desejaSalvar(){
    string entrada;
    if(minhaMusica != NULL && minhaMusica->getTamanhoAtual()!=0){
        do{
            cout << "Existe uma musica aberta. Deseja salvar? ([S]im/[N]ao/[C]ancelar): ";
            cin >> entrada;
        }while(entrada!="S" && entrada!="N" && entrada!="C");
        if(entrada == "C"){
            return false;
        } else if(entrada == "N"){
            delete minhaMusica;
            minhaMusica = NULL;
            return true;
        } else {
            salvar();
            if(minhaMusica != NULL)
                return false;
            else return true;
        }
    } else {
        return true;
    }
}

void BIBLIOTECA::getMusicas(LISTA<string>* musicas){
    getMusicasR(raiz, musicas);
}

void BIBLIOTECA::getMusicasR(NO2<MUSICA*>* no, LISTA<string>* musicas){
    if( no == NULL){
        return;
    } else {
        bool ok;
        musicas->adicionar(no->getInfo()->getNome(), ok);
        getMusicasR(no->getEsq(), musicas);
        getMusicasR(no->getDir(), musicas);
    }
}

void BIBLIOTECA::busca(string s, NO2<MUSICA*>* no, NO2<MUSICA*>* &resultado, bool& encontrado){ // encontrado deve ser false na 1a chamada
    if(no == NULL || encontrado){
        return;
    } else {
        if(no->getInfo()->getNome() == s){
            resultado = no;
            encontrado = true;
            return;
        } else if(no->getInfo()->getNome() < s){
            busca(s, no->getEsq(), resultado, encontrado);
            if(!encontrado){
                busca(s, no->getDir(), resultado, encontrado);
            }
        } else {
            busca(s, no->getDir(), resultado, encontrado);
            if(!encontrado){
                busca(s, no->getEsq(), resultado, encontrado);
            }
        }
    }
}

BIBLIOTECA::~BIBLIOTECA(){

}


void BIBLIOTECA::imprimeAll(){
    cout << "Imprimindo tudo, raiz = "<<raiz<<endl;
    imprimeAllR(raiz);
}
void BIBLIOTECA::imprimeAllR(NO2<MUSICA*> *no){
    if(no!=NULL){
        imprimeAllR(no->getEsq());
        cout << no << " possui " << no->getInfo()->getNome() << ", Dir=" << no->getDir() << ", Esq=" << no->getEsq() << endl;
        imprimeAllR(no->getDir());
    } else
        return;
}

bool BIBLIOTECA::estaNaBiblioteca(string s){
    return estaNaBibliotecaR(s, raiz);
}
bool BIBLIOTECA::estaNaBibliotecaR(string s, NO2<MUSICA*>* no){
    if(no==NULL)
        return false;
    else if(no->getInfo()->getNome()==s)
        return true;
    else
        return (estaNaBibliotecaR(s,no->getDir()) || estaNaBibliotecaR(s,no->getEsq()));
}
