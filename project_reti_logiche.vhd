----------------------------------------------------------------------------------
-- Company: Politecnico di Milano
-- Engineer: Elisabetta Fedele & Filippo Lazzati
-- 
-- Create Date: 13.02.2021 10:12:07
-- Design Name: 
-- Module Name: project_reti_logiche - Behavioral
-- Project Name: 
-- Target Devices: 
-- Tool Versions:
-- Description: 
-- 
-- Dependencies: 
-- 
-- Revision:
-- Revision 0.01 - File Created
-- Additional Comments:
-- 
----------------------------------------------------------------------------------


library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;
use IEEE.STD_LOGIC_UNSIGNED.ALL;
-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx leaf cells in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity project_reti_logiche is
    port ( i_clk : in std_logic;
           i_rst : in std_logic;
           i_start : in std_logic;
           i_data : in std_logic_vector(7 downto 0);
           o_address : out std_logic_vector(15 downto 0);
           o_done : out std_logic;
           o_en : out std_logic;
           o_we : out std_logic; -- 0 per leggere, 1 per scrivere
           o_data : out std_logic_vector(7 downto 0)
           );
end project_reti_logiche;

architecture Behavioral of project_reti_logiche is
    type state_type is (IDLE, COL, ROW, MIN_MAX, ARG, LOG, UPDATE_READ, UPDATE_WRITE);
    signal current_state, next_state : state_type;
    
    signal o_address_next, o_address_reg : std_logic_vector(15 downto 0) := "0000000000000000"; -- in o_address reg il registro attuale, lo stesso ocntenuto di o_address. Ne ho bisogno in quanto o_address non me lo fa leggere
    signal o_data_next: std_logic_vector(7 downto 0) := "00000000";
    signal o_done_next, o_en_next, o_we_next : std_logic := '0';
    signal min: integer range 0 to 255 := 255;
    signal max: integer range 0 to 255 := 0;
    signal count, count_next: integer range 2 to 16385 := 2; --Posso memorizzarli come interi, almeno riesco a fare bene tutte le operazioni che voglio
    signal delta_value: std_logic_vector(7 downto 0) := (others => '0');    
    signal max_address: integer range 2 to 16385 := 2; --è l'ultimo indirizzo in memoria da cui leggo, mi conviene usare direttamente un intero, 
    --farei comunque la conversione per verificare se l'indirizzo a cui sto accedendo è minore o uguale
    signal shift_level: integer range 0 to 8;
    
    begin
        state_reg: process(i_clk, i_rst)
        begin
            if i_rst='1' then
                current_state <= IDLE;
                o_address <= "0000000000000000";
                o_address_reg <= "0000000000000000";
                --o_address_next <= "0000000000000000";
                --count <= 2; --lo inizializzo a 2, tanto mi serve per scorrere i pixel
                --count_next <= 2;
            elsif (i_clk'event and i_clk='1') then --mi dice cosa succede sul fronte di salita del clock a ogni giro
                current_state <= next_state; --aggiorno stato
                o_address <= o_address_next; --aggiorno l'indirizzo a cui leggo/scrivo
                o_address_reg <= o_address_next;  --aggiorno il registro dell'indirizzo 
                o_en <= o_en_next; --dichiaro se voglio l'enable abilitato o meno, in aprticolare qua aggiorno il valore
                o_we <= o_we_next;
                o_data <= o_data_next;
                count <= count_next;
            end if;
        end process;
            
        delta_lambda: process(i_start, i_data, current_state, o_address_reg, min, max, count, max_address)
        variable col_count : integer range 0 to 128 := 0;
        variable i : INTEGER := 0; -- la uso per uscire dal for nello stato LOG
        variable temp_pixel : std_logic_vector(0 to 7) := (others => '0'); -- per il calcolo del new_pixel_value (write_next)
        variable temp_pixel_shifted : std_logic_vector(0 to 15) := (others => '0'); -- per il calcolo del new_pixel_value (write_next)

        begin
            --o_done_next <= '0';
            --o_en_next <= '0';
            --o_we_next <= '0';
            --o_data_next <= "00000000";
            --o_address_next <= "0000000000000000";
            --1° stato: IDLE, è lo stato in cui sto quando non succede niente. Ho un dubbio io metterei o_en_next quando becco lo start. Facendo così secondo me posso leggere subito nello stato successivo!! Sei d'accordo? 
            case current_state is
                when IDLE =>
                    if (i_start = '1') then
                        o_en_next <= '1';
                        o_we_next <= '0';
                        o_done_next <= '0';
                        o_data_next <= "00000000";
                        --o_address_next <= "0000000000000000";
                        next_state <= COL;
                    else
                        next_state <= IDLE;
                        o_done <= '0';
                    end if;
                    
                when COL =>
                    col_count:= conv_integer(i_data);
                    o_address_next <= o_address_reg + "0000000000000001";
                    next_state <= ROW;
                    
                when ROW =>
                    max_address <= (col_count * conv_integer(i_data)) + 1; --dovrei fare +2, ma siccome gli indirizzi partono da 0 faccio solo +1
                    next_state <= MIN_MAX;
                    count_next <= 2; -- la prossima cella che leggo è la 2!
                    o_address_next <= o_address_reg + "0000000000000001";

                
                when MIN_MAX =>
                    -- Aggiorno (se necessario) i valori di massimo e minimo
                    if (conv_integer(i_data) < min) then
                        min <= conv_integer(i_data);
                    elsif (conv_integer(i_data) >max) then
                        max <=  conv_integer(i_data);
                    end if;
                    -- Verifico se l'immagine è finita
                    -- *finita->vado in ARG
                    -- *non finita->mi sposto nella cella successiva
                    if (count = max_address) then
                        count_next <= 2;
                        next_state <= ARG;
                    else
                        count_next <= count + 1;
                        next_state <= MIN_MAX;
                    end if;
                    
                when ARG =>
                    delta_value <= std_logic_vector(to_unsigned(max-min+1, delta_value'length));
                    count_next <= 2;
                    next_state <= LOG;
                    
                when LOG =>
                    i := 0;
                    while (i<8 and delta_value(i) = '0') loop
                        i := i+1;
                    end loop;
                    shift_level <= i;
                    next_state <= UPDATE_READ;
               
                when UPDATE_READ =>
                    if (shift_level = 0) then
                        temp_pixel_shifted := "00000000" & i_data;
                    elsif (shift_level = 1) then
                        temp_pixel_shifted := "0000000" & i_data & "0";
                    elsif (shift_level = 2) then
                        temp_pixel_shifted := "000000" & i_data & "00";
                    elsif (shift_level = 3) then
                        temp_pixel_shifted := "00000" & i_data & "000";
                    elsif (shift_level = 4) then
                        temp_pixel_shifted := "0000" & i_data & "0000";
                    elsif (shift_level = 5) then
                        temp_pixel_shifted := "000" & i_data & "00000";
                    elsif (shift_level = 6) then
                        temp_pixel_shifted := "00" & i_data & "000000";
                    elsif (shift_level = 7) then
                        temp_pixel_shifted := "0" & i_data & "0000000";
                    else 
                        temp_pixel_shifted := i_data & "00000000";
                     
                    
                    
                   -- else
                     --   for i in 15 downto 15-shift_level+1 loop
                       --     temp_pixel_shifted(i) := '0';
                         --   end loop;
                        --for i in 15-shift_level downto 8-shift_level loop
                          --  temp_pixel_shifted (i) := i_data (i+shift_level-8);
                            --end loop;
                        --if(shift_level < 8) then
                          --  for i in 8-shift_level-1 downto 0 loop
                            ----    temp_pixel_shifted (i) := '0';
                                --end loop;
                       -- end if;
                    end if;
                    
                        
                    --temp_pixel_shifted := std_logic_vector(temp_pixel sll shift_level);
                    if conv_integer(temp_pixel_shifted) > 255 then
                        o_data_next <= "11111111";
                    else
                        for i in 15 downto 8 loop
                            o_data_next(i-8) <= temp_pixel_shifted (i);
                        end loop;
                    end if;
                    o_address_next <= o_address_reg + std_logic_vector(to_unsigned(max_address - 1, o_address_next'length));
                    o_we_next <= '1';
                    next_state <= UPDATE_WRITE;
                    
                when UPDATE_WRITE =>
                    if(to_integer(unsigned(o_address_reg)) = max_address + max_address - 1) then
                    --if (count = max_address) then
                        count_next <= 2;
                        o_done_next <= '1';
                        next_state <= IDLE;
                    else
                        count_next <= count + 1;
                        o_address_next <= o_address_reg - std_logic_vector(to_unsigned(max_address - 2, o_address_next'length));
                        o_we_next <= '0';
                        next_state <= UPDATE_READ;
                    end if;
            end case;
        end process;
                        
                       
end Behavioral;
