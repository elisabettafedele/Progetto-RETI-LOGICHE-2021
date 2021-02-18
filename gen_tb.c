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

void stampa(int n_immagini,int** img,int** img_eq,int* row,int* col){
	int i,j,pixel;
	for(i=0;i<n_immagini;i++){
		pixel=row[i]*col[i];
		printf("\nimmagine vera numero %d:\n",i+1);
		for(j=0;j<pixel;j++)
			printf("%d ",img[i][j]);
		printf("\nimmagine equalizzata numero %d\n",i+1);
		for(j=0;j<pixel;j++)
			printf("%d ",img_eq[i][j]);		
	}
}


int main(){
	
	time_t t; // per il random
	srand((unsigned) time(&t));
	
	FILE* fp = fopen ("tb.txt","w"); // cambia in .txt se vuoi leggere i risultati !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	int i,j,pixel,schifo,delta,shift,temp,k;
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
	printf("Inserisci numero immagini\n");
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
	int min[n_immagini],max[n_immagini];
	
	for (i=0; i<n_immagini;i++){
		min[i]=255;
		max[i]=0;
		printf("Inserisci il numero di righe dell'immagine %d\n",i+1);
		scanf("%d",&row[i]);
		printf("Inserisci il numero di colonne dell'immagine %d\n",i+1);
		scanf("%d",&col[i]);
		pixel=row[i]*col[i];
		immagini[i] = malloc(pixel * sizeof(int));
		img_equalizzate[i] = malloc(pixel * sizeof(int));
		for(j=0;j<pixel;j++){
			schifo = rand() % 255;
			immagini[i][j]=schifo;
			if(schifo<min[i])
				min[i]=schifo;
			if(schifo>max[i])
				max[i]=schifo;			
		}
		delta=max[i]-min[i]+1;
		shift=8-log2(delta);
		for(j=0;j<pixel;j++){
			temp=immagini[i][j]-min[i];
			temp=shift_f(temp,shift);
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
    fprintf(fp,"wait until tb_done = '1';\n");
    fprintf(fp,"wait for c_CLOCK_PERIOD;\n");
	fprintf(fp,"tb_start <= '0';\n");
	fprintf(fp,"wait until tb_done = '0';\n");
	fprintf(fp,"wait for 100 ns;\n");
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
	stampa(n_immagini,immagini,img_equalizzate,row,col);
	
	fclose(fp);
	return 0;
}
