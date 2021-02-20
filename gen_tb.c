#include<stdio.h>
#include<stdlib.h>

int log2 (int n){
	int a=256;
	int i;
	for(i=8;i>0;i--){
		if(n/(a) == 1) return i;
		a=a/2; 
	}
	return 0;
}

int shift_f (int a, int shift){
	int i,n;
	n=a;
	for(i=0;i<shift;i++)
	n=n*2;
	return n;
}

void stampa(int n_immagini,int** img,int** img_eq,int* row,int* col,int *min, int *max, int *delta, int *shift){
	int i,j,pixel;
	for(i=0;i<n_immagini;i++){
		pixel=row[i]*col[i];
		printf("immagine numero %d:\nmin: %d, max: %d, delta+1: %d, shift: %d\n",i+1,min[i],max[i],delta[i],shift[i]);
		/*for(j=0;j<pixel;j++)
			printf("%d ",img[i][j]);
		printf("\nimmagine equalizzata numero %d\n",i+1);
		for(j=0;j<pixel;j++)
			printf("%d ",img_eq[i][j]);	*/	
	
}
}


int main(){
	
	time_t t; // per il random
	srand((unsigned) time(&t));
	FILE *f1;
	FILE* fp = fopen ("tb.vhd","w"); // cambia in .txt se vuoi leggere i risultati !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	int i,j,pixel,schifo,temp,k,scelta,bob,mike;
	int clock;
	printf("Scegli il clock in nanosecondi\n");
	scanf("%d",&clock);
	
// ARCHITECTURE
	fprintf(fp,"\nlibrary ieee;\nuse ieee.std_logic_1164.all;\nuse ieee.numeric_std.all;\nuse ieee.std_logic_unsigned.all;\n\n\n");
	fprintf(fp,"entity project_tb is\nend project_tb;\n\n\narchitecture projecttb of project_tb is\nconstant c_CLOCK_PERIOD: time := %d ns;\n",clock);
	fprintf(fp,"signal   tb_done                : std_logic;\n");
	fprintf(fp,"signal   mem_address            : std_logic_vector (15 downto 0) := (others => '0');\n");
	fprintf(fp,"signal   tb_rst                 : std_logic := '0';\n");
	fprintf(fp,"signal   tb_start               : std_logic := '0';\n");
	fprintf(fp,"signal   tb_clk                 : std_logic := '0';\n");
	fprintf(fp,"signal   mem_o_data,mem_i_data  : std_logic_vector (7 downto 0);\n");
	fprintf(fp,"signal   enable_wire            : std_logic;\n");
	fprintf(fp,"signal   mem_we                 : std_logic;\n\n");
	fprintf(fp,"type ram_type is array (65535 downto 0) of std_logic_vector(7 downto 0);\n\n");
//RAM	
	int n_immagini;
	char tab;
	printf("Inserisci numero immagini (se vuoi reset asincrono inserisci 1)\n");
	scanf("%d",&n_immagini);
	if(n_immagini>1){
	fprintf(fp,"signal i: std_logic_vector(%d downto 0) := \"",n_immagini-1);
	for(i=0;i<n_immagini;i++)
	fprintf(fp,"0");
	fprintf(fp,"\";\n");
	}
	fprintf(fp,"\n\n");
	
	int* immagini[n_immagini];
	int* img_equalizzate[n_immagini];
	int row[n_immagini],col[n_immagini];
	int min[n_immagini],max[n_immagini], delta[n_immagini],shift[n_immagini];
	
	for (i=0; i<n_immagini;i++){
		scelta=0;
		printf("digita 1 per importare un'immagine 128x128pixel da un file \"milano_bn.txt\", 0 altrimenti\n");
		scanf("%d",&scelta);
		if(scelta==0){
		printf("Inserisci il numero di righe dell'immagine %d\n",i+1);
		scanf("%d",&row[i]);
		printf("Inserisci il numero di colonne dell'immagine %d\n",i+1);
		scanf("%d",&col[i]);
		pixel=row[i]*col[i];
		immagini[i] = malloc(pixel * sizeof(int));
		img_equalizzate[i] = malloc(pixel * sizeof(int));
		printf("Digita 0 per inserimento random, 1 per scegliere min e max (se righe*colonne=1 non scegliere 1), 2 per scegliere tutti i valori\n");
		scanf("%d",&scelta);
		if(scelta==0){
		min[i]=255;
		max[i]=0;
		for(j=0;j<pixel;j++){
			schifo = rand() % 255;
			immagini[i][j]=schifo;
			if(schifo<min[i])
				min[i]=schifo;
			if(schifo>max[i])
				max[i]=schifo;			
		}}
		if(scelta==1){
			printf("Inserisci min:\n");
			scanf("%d",&min[i]);
			printf("Inserisci max:\n");
			scanf("%d",&max[i]);
			immagini[i][0]=min[i];
			immagini[i][1]=max[i];
			for(j=2;j<pixel;j++){
			do{
				schifo = rand() % 255;
			}while(schifo<min[i] || schifo>max[i]);
			immagini[i][j]=schifo;			
		}	
		}
		if(scelta==2){
			min[i]=255;
			max[i]=0;
			for(j=0;j<pixel;j++){
				printf("Inserisci pixel numero %d:\n",j+1);
				scanf("%d",&schifo);
				immagini[i][j]=schifo;
				if(schifo<min[i])
				min[i]=schifo;
				if(schifo>max[i])
				max[i]=schifo;			
		}	
		}}
		else{
			row[i]=128;
			col[i]=128;
			min[i]=255;
			max[i]=0;
			pixel=128*128;
			immagini[i] = malloc(pixel * sizeof(int));
			img_equalizzate[i] = malloc(pixel * sizeof(int));
			f1=fopen("milano_bn.txt","r");
	for(j=0;j<pixel;j++){
		fscanf(f1,"%d",&schifo);
		immagini[i][j]=schifo;
		if(schifo<min[i])
		min[i]=schifo;
		if(schifo>max[i])
		max[i]=schifo;
		fscanf(f1,"%c",&tab);
	}
	fclose(f1);
		printf("Inserisci 1 per disequalizzare questa immagine, 0 altrimenti\n");
		scanf("%d",&scelta);
		if(scelta==1){
			printf("Inserisci valore minimo:\n");
			scanf("%d",&min[i]);
			printf("Inserisci valore massimo:\n");
			scanf("%d",&max[i]);
			for(j=0;j<pixel;j++){
		if(immagini[i][j]<min[i])
		immagini[i][j]=min[i];
		if(immagini[i][j]>max[i])
		immagini[i][j]=max[i];
		}
		}
		}
		delta[i]=max[i]-min[i]+1;
		shift[i]=8-log2(delta[i]);
		for(j=0;j<pixel;j++){
			temp=immagini[i][j]-min[i];
			temp=shift_f(temp,shift[i]);
			if(temp>255)
			img_equalizzate[i][j]=255;
			else
			img_equalizzate[i][j]=temp;
		}
	}
	if(n_immagini ==1){
			k=0;
		fprintf(fp,"signal RAM: ram_type := ( ");
		pixel = row[0]*col[0];
		fprintf(fp,"%d => std_logic_vector(to_unsigned(  %d  , 8)),\n",k,col[0]);
		k++;
		fprintf(fp,"%d => std_logic_vector(to_unsigned(  %d  , 8)),\n",k,row[0]);
		k++;
		for(j=0;j<pixel;j++){
			fprintf(fp,"%d => std_logic_vector(to_unsigned(  %d  , 8)),\n",k,immagini[0][j]);
			k++;
		}
	fprintf(fp,"others => (others =>'0'));\n\n");
	}
	else{
	for(i=0;i<n_immagini;i++){
		k=0;
		fprintf(fp,"signal RAM%d: ram_type := ( ",i+1);
		pixel = row[i]*col[i];
		fprintf(fp,"%d => std_logic_vector(to_unsigned(  %d  , 8)),\n",k,col[i]);
		k++;
		fprintf(fp,"%d => std_logic_vector(to_unsigned(  %d  , 8)),\n",k,row[i]);
		k++;
		for(j=0;j<pixel;j++){
			fprintf(fp,"%d => std_logic_vector(to_unsigned(  %d  , 8)),\n",k,immagini[i][j]);
			k++;
		}
	fprintf(fp,"others => (others =>'0'));\n\n");
}
}
//COMPONENT
	fprintf(fp,"component project_reti_logiche is\n");
	fprintf(fp,"port (\n");
	fprintf(fp,"i_clk         : in  std_logic;\n");
	fprintf(fp,"i_rst         : in  std_logic;\n");
	fprintf(fp,"i_start       : in  std_logic;\n");
	fprintf(fp,"i_data        : in  std_logic_vector(7 downto 0);\n");
	fprintf(fp," o_address     : out std_logic_vector(15 downto 0);\n");
	fprintf(fp,"o_done        : out std_logic;\n");
	fprintf(fp,"o_en          : out std_logic;\n");
	fprintf(fp,"o_we          : out std_logic;\n");
	fprintf(fp,"o_data        : out std_logic_vector (7 downto 0)\n");
	fprintf(fp,");\n");
	fprintf(fp,"end component project_reti_logiche;\n\n\n");
//BEGIN
	fprintf(fp,"begin\n");
	fprintf(fp,"UUT: project_reti_logiche\n");
	fprintf(fp,"port map (\n");
	fprintf(fp,"i_clk      	=> tb_clk,\n");
	fprintf(fp," i_rst      	=> tb_rst,\n");
	fprintf(fp,"i_start       => tb_start,\n");
	fprintf(fp,"i_data    	=> mem_o_data,\n");
	fprintf(fp,"o_address  	=> mem_address,\n");
	fprintf(fp,"o_done      	=> tb_done,\n");
	fprintf(fp,"o_en   	=> enable_wire,\n");
	fprintf(fp,"o_we 		=> mem_we,\n");
	fprintf(fp,"o_data    	=> mem_i_data\n");
	fprintf(fp,");\n\n");
//P CLK GEN PROCESS
    fprintf(fp,"p_CLK_GEN : process is\n");
	fprintf(fp,"begin\n");
	fprintf(fp,"wait for c_CLOCK_PERIOD/2;\n");
	fprintf(fp,"tb_clk <= not tb_clk;\n");
	fprintf(fp,"end process p_CLK_GEN;\n\n\n");
// MEM PROCESS
    fprintf(fp,"MEM : process(tb_clk)\n");
    fprintf(fp,"begin\n");
	fprintf(fp,"if tb_clk'event and tb_clk = '1' then\n");
	fprintf(fp,"if enable_wire = '1' then\n");
	if(n_immagini==1){
	fprintf(fp,"if mem_we = '1' then\n");
    fprintf(fp,"RAM(conv_integer(mem_address))  <= mem_i_data;\n");
    fprintf(fp,"mem_o_data                      <= mem_i_data after 1 ns;\n");
	fprintf(fp,"else\n");
	fprintf(fp,"mem_o_data <= RAM(conv_integer(mem_address)) after 1 ns;\n");
	}
	else{
		for(i=0;i<n_immagini;i++){
			if(i==0)
				fprintf(fp,"if i = \"");
			else
				fprintf(fp,"elsif i = \"");
			for(j=0;j<n_immagini;j++){
				if(j == n_immagini-i-1)
					fprintf(fp,"1");
				else
					fprintf(fp,"0");
			}
			fprintf(fp,"\" then\n");
			fprintf(fp,"if mem_we = '1' then\n");
    		fprintf(fp,"RAM%d(conv_integer(mem_address))  <= mem_i_data;\n",i+1);
    		fprintf(fp,"mem_o_data                      <= mem_i_data after 1 ns;\n");
			fprintf(fp,"else\n");
			fprintf(fp,"mem_o_data <= RAM%d(conv_integer(mem_address)) after 1 ns;\n",i+1);
			fprintf(fp,"end if;\n");
		}
	}
	fprintf(fp,"end if;\n");
	fprintf(fp,"end if;\n");
	fprintf(fp,"end if;\n");
    fprintf(fp,"end process;\n\n\n");
// TEST PROCESS
	fprintf(fp,"test : process is\n");
    fprintf(fp,"begin \n");
	if(n_immagini == 1){
	fprintf(fp,"wait for 100 ns;\n");
	fprintf(fp,"wait for c_CLOCK_PERIOD;\n");
	fprintf(fp,"tb_rst <= '1';\n");
    fprintf(fp,"wait for c_CLOCK_PERIOD;\n");
    fprintf(fp,"wait for 100 ns;\n");
	fprintf(fp,"tb_rst <= '0';\n");
	fprintf(fp,"wait for c_CLOCK_PERIOD;\n");
	fprintf(fp,"wait for 100 ns;\n");
	fprintf(fp,"tb_start <= '1';\n");
    fprintf(fp,"wait for c_CLOCK_PERIOD;\n");
    printf("Digita 1 per far si' che il test_bench improvvisamente setti i_rst a 1, digita 0 per non farlo, digita 2 per far si'\n");
    printf("che il setting a 1 di i_rst avvenga ripetutamente\n");
    scanf("%d",&scelta);
    if(scelta==0){
    fprintf(fp,"wait until tb_done = '1';\n");
    fprintf(fp,"wait for c_CLOCK_PERIOD;\n");
	fprintf(fp,"tb_start <= '0';\n");
	fprintf(fp,"wait until tb_done = '0';\n");
	fprintf(fp,"wait for 100 ns;\n");
	}
	if(scelta==1){
	printf("Inserisci dopo quanto tempo dal momento in cui i_start è andato a 1 vuoi che i_rst vada a 1 (in ns)\n");
	scanf("%d",&bob);
	fprintf(fp,"wait for %d ns;\n",bob);
	fprintf(fp,"tb_rst <= '1';\n");
    fprintf(fp,"wait for c_CLOCK_PERIOD;\n");
    fprintf(fp,"wait for 100 ns;\n");
	fprintf(fp,"tb_rst <= '0';\n");
	fprintf(fp,"wait for c_CLOCK_PERIOD;\n");
	fprintf(fp,"wait for 100 ns;\n");
	fprintf(fp,"tb_start <= '1';\n");
    fprintf(fp,"wait for c_CLOCK_PERIOD;\n");
    fprintf(fp,"wait until tb_done = '1';\n");
    fprintf(fp,"wait for c_CLOCK_PERIOD;\n");
	fprintf(fp,"tb_start <= '0';\n");
	fprintf(fp,"wait until tb_done = '0';\n");
	fprintf(fp,"wait for 100 ns;\n");
	}
	if(scelta==2){
		printf("Inserisci quante volte vuoi che i_rst vada a 1\n");
		scanf("%d",&mike);
		printf("Ok, ora genero %d istanti di tempo casuali >= 0 (incluso) ns corrispondenti all'intervallo di tempo\n",mike);
		printf("fra cui i_start va a 1 e i_rst va a 1:\n");
		for(i=0;i<mike;i++){
			bob = rand() % 255;
			fprintf(fp,"wait for %d ns;\n",bob);
			fprintf(fp,"tb_rst <= '1';\n");
    		fprintf(fp,"wait for c_CLOCK_PERIOD;\n");
    		fprintf(fp,"wait for 100 ns;\n");
			fprintf(fp,"tb_rst <= '0';\n");
			fprintf(fp,"wait for c_CLOCK_PERIOD;\n");
			fprintf(fp,"wait for 100 ns;\n");
			fprintf(fp,"tb_start <= '1';\n");
    		fprintf(fp,"wait for c_CLOCK_PERIOD;\n");
		}
		fprintf(fp,"wait until tb_done = '1';\n");
    	fprintf(fp,"wait for c_CLOCK_PERIOD;\n");
		fprintf(fp,"tb_start <= '0';\n");
		fprintf(fp,"wait until tb_done = '0';\n");
		fprintf(fp,"wait for 100 ns;\n");
	}
	}
	else{
		for(i=0;i<n_immagini;i++){			
			fprintf(fp,"i <= \"");
				for(j=0;j<n_immagini;j++){
				if(j == n_immagini-i-1)
					fprintf(fp,"1");
				else
					fprintf(fp,"0");
			}
			fprintf(fp,"\";\n");
			if(i!=0) fprintf(fp, "\n\n");
			else{
				fprintf(fp,"wait for 100 ns;\n");
				fprintf(fp,"wait for c_CLOCK_PERIOD;\n");
				fprintf(fp,"tb_rst <= '1';\n");
    			fprintf(fp,"wait for c_CLOCK_PERIOD;\n");
    			fprintf(fp,"wait for 100 ns;\n");
				fprintf(fp,"tb_rst <= '0';\n");
				fprintf(fp,"wait for c_CLOCK_PERIOD;\n");
			}
			fprintf(fp,"wait for 100 ns;\n");
			fprintf(fp,"tb_start <= '1';\n");
    		fprintf(fp,"wait for c_CLOCK_PERIOD;\n");
    		fprintf(fp,"wait until tb_done = '1';\n");
    		fprintf(fp,"wait for c_CLOCK_PERIOD;\n");
			fprintf(fp,"tb_start <= '0';\n");
			fprintf(fp,"wait until tb_done = '0';\n");
			fprintf(fp,"wait for 100 ns;\n");
	}}
fprintf(fp,"\n");
if(n_immagini == 1){
pixel=row[0]*col[0];
k=2+pixel;
for(j=0;j<pixel;j++){
fprintf(fp,"assert RAM(%d) = std_logic_vector(to_unsigned( %d , 8)) report \"TEST FALLITO (WORKING ZONE). Expected  %d  found\"",k,img_equalizzate[0][j],img_equalizzate[0][j]);
fprintf(fp," & integer'image(to_integer(unsigned(RAM(%d))))  severity failure;\n",k);
k++;
}
fprintf(fp,"\n\n\n");
}
else{
for(i=0;i<n_immagini;i++){
pixel=row[i]*col[i];
k=2+pixel;
for(j=0;j<pixel;j++){
fprintf(fp,"assert RAM%d(%d) = std_logic_vector(to_unsigned( %d , 8)) report \"TEST FALLITO (WORKING ZONE). Expected  %d  found\"",i+1,k,img_equalizzate[i][j],img_equalizzate[i][j]);
fprintf(fp," & integer'image(to_integer(unsigned(RAM%d(%d))))  severity failure;\n",i+1,k);
k++;
}
fprintf(fp,"\n");
if(i==n_immagini-1)
	fprintf(fp,"\n\n");
}
}

    fprintf(fp,"assert false report \"Simulation Ended! TEST PASSATO\" severity failure;\n");
    fprintf(fp,"end process test;\n");
	fprintf(fp,"end projecttb;\n");
	
	fclose(fp);
//CREA IMMAGINE/I:
scelta=0;
printf("Digita 1 per creare due file di testo per ogni immagine, uno con tutti i pixel,\n");
printf("e uno con tutti quelli dell'immagine equalizzata, altrimenti digita 0\n");
scanf("%d",&scelta);
int indice;
char c;
char s[11]="imagex.txt";
char s_eq[13]="image_eqx.txt";
FILE *f = NULL;
FILE *f_eq = NULL;
if(scelta==1){
for(i=0;i<n_immagini;i++){
	s[5]=i+49;
	s_eq[8]=i+49;
f = fopen(s,"w");
f_eq = fopen(s_eq,"w");
indice=0;
for(j=0;j<row[i];j++){
	for(k=0;k<col[i];k++){
	fprintf(f,"%d",immagini[i][indice]);
	fprintf(f_eq,"%d ",img_equalizzate[i][indice]);
	indice++;
	if(k!=col[i]-1){
		fprintf(f,"\t");
		fprintf(f_eq,"\t");
	}
	}
fprintf(f,"\n");
fprintf(f_eq,"\n");
}
fclose(f);
fclose(f_eq);
}
}
stampa(n_immagini,immagini,img_equalizzate,row,col,min,max,delta,shift);
	return 0;
}
