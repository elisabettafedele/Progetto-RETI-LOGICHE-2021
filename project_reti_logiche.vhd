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
    type state_type is (IDLE, COL, ROW, MIN_MAX, ARG, LOG, UPDATE_READ, UPDATE_WRITE, DONE);
    signal current_state, next_state : state_type;
    signal o_address_next, o_address_reg : std_logic_vector(15 downto 0) := "0000000000000000"; -- in o_address reg il registro attuale, lo stesso ocntenuto di o_address. Ne ho bisogno in quanto o_address non me lo fa leggere
    signal o_data_next: std_logic_vector(7 downto 0) := "00000000";
    signal o_done_next, o_en_next, o_we_next : std_logic := '0';
    signal min, min_next: integer range 0 to 255 := 255;
    signal max, max_next: integer range 0 to 255 := 0;
    signal count, count_next: integer range 0 to 16385 := 2; --Posso memorizzarli come interi, almeno riesco a fare bene tutte le operazioni che voglio
    signal delta_value, delta_value_next: std_logic_vector(8 downto 0) := (others => '0');    
    signal max_address, max_address_next: integer range 0 to 16385 := 2;
    signal shift_level, shift_level_next: integer range 0 to 8;
    
    begin
        state_reg: process(i_clk, i_rst)
        begin
            if i_rst='1' then
                delta_value <= (others => '0');
                max_address <= 2;
                current_state <= IDLE;
                o_address <= "0000000000000000";
                o_address_reg <= "0000000000000000";
                count <= 2;
                max <= 0;
                min <= 255;
                o_done <= '0';
                o_en <= '1';
                o_we <= '0';
                o_data <= "00000000";
                
            elsif (i_clk'event and i_clk='1') then --mi dice cosa succede sul fronte di salita del clock a ogni giro
                current_state <= next_state; --aggiorno stato
                o_address <= o_address_next; --aggiorno l'indirizzo a cui leggo/scrivo
                o_address_reg <= o_address_next;  --aggiorno il registro dell'indirizzo 
                o_en <= o_en_next; --dichiaro se voglio l'enable abilitato o meno, in aprticolare qua aggiorno il valore
                o_we <= o_we_next;
                o_data <= o_data_next;
                count <= count_next;
                o_done <= o_done_next;
                max <= max_next;
                min <= min_next;
                max_address <= max_address_next;
                shift_level <= shift_level_next;
                delta_value <= delta_value_next;
            end if;
        end process;
            
        delta_lambda: process(i_start, i_data, current_state, o_address_reg, min, max, count, max_address, current_state)
        variable col_count : integer range 0 to 128 := 0;
        variable i : INTEGER := 0; -- la uso per uscire dal for nello stato LOG
        variable temp_pixel : std_logic_vector(7 downto 0) := (others => '0'); -- per il calcolo del new_pixel_value (write_next)
        variable temp_pixel_shifted : std_logic_vector(15 downto 0) := (others => '0'); -- per il calcolo del new_pixel_value (write_next)

        begin
            o_done_next <= '0';
            o_en_next <= '1';
            o_we_next <= '0';
            o_data_next <= "00000000";
            o_address_next <= "0000000000000000";
            count_next <= count;
            max_next <= max;
            min_next <= min;
            max_address_next <= max_address;
            shift_level_next <= shift_level;
            delta_value_next <= delta_value;
            case current_state is
                when IDLE =>
                    if (i_start = '1') then
                        next_state <= COL;
                    else
                        next_state <= IDLE;
                    end if;
                    
                when COL =>
                    col_count:= to_integer(unsigned(i_data));
                    o_address_next <= "0000000000000001";
                    next_state <= ROW;
                    count_next<=2;
                when ROW =>
                    max_address_next <= (col_count * to_integer(unsigned(i_data))) + 1; --dovrei fare +2, ma siccome gli indirizzi partono da 0 faccio solo +1
                    count_next <= 3; -- la prossima cella che leggo è la 2, ma in verità poi comando la 3!
                    o_address_next <= "0000000000000010";
                    next_state <= MIN_MAX;      
                when MIN_MAX =>
                    if (to_integer(unsigned(i_data)) < min) then
                        min_next <= to_integer(unsigned(i_data));
                    end if;
                    if (to_integer(unsigned(i_data)) > max) then
                        max_next <=  to_integer(unsigned(i_data));
                    end if;
                    if (count = max_address+1) then
                        o_address_next <= "0000000000000010";
                        --o_address_next <= std_logic_vector(resize(to_unsigned(min, 9),16));
                        
                        min_next <= 46; -- !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! toglila dopo
                        o_en_next <= '0';
                        next_state <= ARG;
                    else
                        count_next <= count + 1;
                        o_address_next <= std_logic_vector(to_unsigned(count, 16));
                        next_state <= MIN_MAX;
                    end if;
                    
                when ARG =>
                    delta_value_next <= std_logic_vector(to_unsigned(max-min+1, 9));
                    --o_address_next <= std_logic_vector(resize(to_unsigned(max-min+1, 9),16));
                    o_en_next <= '0';
                    next_state <= LOG;
                    
                when LOG =>
                    i := 0;
                    while (i<9 and delta_value(i) = '0') loop
                        i := i+1;
                    end loop;
                    shift_level_next <= i;
                    count_next <= 1;
                    o_address_next <= "0000000000000010";
                    next_state <= UPDATE_READ;
               
                when UPDATE_READ =>
                    temp_pixel := std_logic_vector(unsigned(i_data) - to_unsigned(min, 8));
                    temp_pixel_shifted := std_logic_vector(resize(unsigned(temp_pixel), 16));
                    temp_pixel_shifted := std_logic_vector(shift_left(unsigned(temp_pixel_shifted), shift_level));
                    
                    if to_integer(unsigned(temp_pixel_shifted)) > 255 then
                        o_data_next <= "11111111";
                    else
                        o_data_next <= std_logic_vector(resize(unsigned(temp_pixel_shifted), 8));
                    end if;
                    o_address_next <= std_logic_vector (to_unsigned(max_address+count, 16));
                    o_we_next <= '1';
                    next_state <= UPDATE_WRITE;
                    
                when UPDATE_WRITE =>
                    if(count = max_address - 1) then
                        o_done_next <= '1';
                        count_next <= 2;
                        o_en_next <= '0';
                        min_next <= 255;
                        max_next <= 0;
                        max_address_next <= 0;
                        shift_level_next <= 0;
                        delta_value_next <= "000000000";
                        next_state <= DONE;
                    else
                        count_next <= count + 1;
                        o_address_next <= std_logic_vector (to_unsigned(2 + count, 16));
                        next_state <= UPDATE_READ;
                    end if;
                
                when DONE =>
                    if (i_start = '0') then                        
                        o_en_next <= '0';                                
                        next_state <= IDLE;
                    else
                        o_en_next <= '0';
                        next_state <= DONE;
                    end if;
            end case;
        end process;
                        
                       
end Behavioral;