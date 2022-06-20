/*-------------------Variaveis globais-----------------*/
unsigned int tempo =0;
unsigned int tempoAnterior=0;
char tempoLido[]="";
unsigned int intervaloTempo=1;
int tempo_medicao = 10000;
char temperatura[10]={"0"};
char umidade[10]={"0"};	
int  pressao=0,luminosidade=0;
//Variaveis de historico das medicoes
Dados historico[MAX]; 
Dados historico_display[MAX];
//Variaveis de controle da lista do historico
int L=-1;
int O=-1;
/*----------------------------------------------------*/

//Adiciona uma nova item(medida) no historico. Quando a lista possuir 10 itens, o mais antigo e sobrescrito.
void add(int lum, int press, char *temp , char *umi){
    if(L== -1){     //Define a posicao de insercao do elemento se a lista estiver vazia
        L=0;    //A medicao mais recente eh a primeira posicao
        O=0;    //A medicao mais antiga eh a primeira posicao
    }
    else{  //Define a posicao de insercao do elemento se a lista nao estiver vazia
        if(L == MAX-1){
            L=0;
            O++;
        }
        else if(L<O){
            L++;
            O++;
        }
        else
            L++;
    }
    //Insere os novos itens na posicao definida acima
    historico[L].lumi= lum;
    historico[L].press= press;
    strcpy(historico[L].temp,temp);
    strcpy(historico[L].umi,umi);
    getDataTempo(historico[L].data_hora_atual); // obtem data e hora da medicao
}

//--------------------------------------------------------------------------
//Cria um vetor ordenado da medicao mais recente para a mais antiga
void getOrdenada(Dados *v){
    int i,idx;
    idx= L;
    for(i=0;i<MAX;i++){
        v[i]=historico[idx];
        if(idx !=O && idx ==0)
            idx= MAX-1;
        else
            idx--;
    }
}
//---------------------------------------------------------------------------
void escrever_arquivo(Dados *data){
	FILE *pont_arq;	// ponteiro para o arquivo
	pont_arq = fopen("historico.txt", "w"); //Cria arquivo para escrita
	int i=0;
	
	if(pont_arq == NULL){
		printf("Erro na abertura");
		exit(1);
	}
	
	for(i=0;i<10;i++){
		fputs("---------------\n",pont_arq);
		fprintf(pont_arq,"Temperatura=%.3f\n",data[i].temp);
		fprintf(pont_arq,"Umidade=%.3f\n",data[i].umi);
		fprintf(pont_arq,"Luminosidade=%d\n",data[i].lumi);
		fprintf(pont_arq,"Pressao=%d\n",data[i].press);
		fprintf(pont_arq,"Data %d|%d|%d Hora %d:%d:%d \n",data[i].data_hora_atual->tm_mday,data[i].data_hora_atual->tm_mon+1,data[i].data_hora_atual->tm_year+1900
		,data[i].data_hora_atual->tm_hour,data[i].data_hora_atual->tm_min,data[i].data_hora_atual->tm_sec);
	}
	
	fclose(pont_arq);
}
