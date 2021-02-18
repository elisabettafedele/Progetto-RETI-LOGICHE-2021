----------------------------------------------------------------------------------
--
-- Prova Finale (Progett di Reti Logiche)
-- Prof. Fabio Salice - Anno Accademico 2020/2021
--
-- Elisabetta Fedele (Codice Persona 10631762 Matricola 911320)
-- Filippo Lazzati (Codice Persona 10629918 Matricola 910614)
-- 
----------------------------------------------------------------------------------

library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;
use IEEE.STD_LOGIC_UNSIGNED.ALL;

entity project_reti_logiche is
    port (
           i_clk : in std_logic;
           i_rst : in std_logic;
           i_start : in std_logic;
           i_data : in std_logic_vector(7 downto 0);
           o_address : out std_logic_vector(15 downto 0);
           o_done : out std_logic;
           o_en : out std_logic;
           o_we : out std_logic;
           o_data : out std_logic_vector(7 downto 0)
           );
end project_reti_logiche;

architecture Behavioral of project_reti_logiche is

    type state_type is (IDLE, COL, ROW, WAIT_VALUE, MIN_MAX, ARG, LOG, UPDATE_READ, UPDATE_WRITE, DONE);
    signal current_state, next_state : state_type;
    signal o_address_next, o_address_reg : std_logic_vector(15 downto 0) := (others => '0'); 
    signal o_data_next: std_logic_vector(7 downto 0) := (others => '0');
    signal o_done_next, o_en_next, o_we_next : std_logic := '0';
    signal min, min_next: integer range 0 to 255 := 255;
    signal max, max_next: integer range 0 to 255 := 0;
    signal count, count_next: integer range 0 to 16385 := 2; 
    signal delta_value, delta_value_next: std_logic_vector(8 downto 0) := (others => '0');    
    signal max_address, max_address_next: integer  range 0 to 16385 := 2;
    signal shift_level, shift_level_next: integer  range 0 to 8 := 0;
    signal state_return,state_return_next : state_type;
    
    begin
        state_reg: process(i_clk, i_rst)
        begin
        
            if (i_rst='1') then
                o_address <= (others => '0');
                o_done <= '0';
                o_en <= '0';
                o_we <= '0';
                o_data <= (others => '0');                
                current_state <= IDLE;
                o_address_reg <= (others => '0');
                min <= 255; 
                max <= 0;
                count <= 2;        
                delta_value <= (others => '0');
                max_address <= 2;
                shift_level <= 0;                
                state_return <= IDLE;
                          
            elsif (rising_edge(i_clk)) then 
                o_address <= o_address_next;
                o_done <= o_done_next;
                o_en <= o_en_next; 
                o_we <= o_we_next;
                o_data <= o_data_next;                
                current_state <= next_state;                 
                o_address_reg <= o_address_next;
                min <= min_next; 
                max <= max_next;                
                count <= count_next;
                delta_value <= delta_value_next;                
                max_address <= max_address_next;
                shift_level <= shift_level_next;                
                state_return <= state_return_next;
                
            end if;
        end process;
            
        delta_lambda: process(i_start, i_data, current_state, o_address_reg, min, max, count, max_address, shift_level, delta_value)
        variable col_count : integer range 0 to 128 := 0;
        variable i : integer := 0;
        variable temp_pixel : std_logic_vector(7 downto 0) := (others => '0'); 
        variable temp_pixel_shifted : std_logic_vector(15 downto 0) := (others => '0'); 

        begin
            o_address_next <= (others => '0');
            o_done_next <= '0';
            o_en_next <= '0';
            o_we_next <= '0';
            o_data_next <= (others => '0');            
            min_next <= min;
            max_next <= max;            
            count_next <= count;    
            delta_value_next <= delta_value;
            max_address_next <= max_address;
            shift_level_next <= shift_level;
                        
            case current_state is
                when IDLE =>
                    if (i_start = '1') then          
                        o_en_next <= '1';
                        state_return_next <= COL;
                        next_state <= WAIT_VALUE;
                    else
                        next_state <= IDLE;
                    end if;
                when WAIT_VALUE =>
                    o_address_next <= o_address_reg;                    
                    o_en_next <= '1';
                    next_state <= state_return;                  
                when COL =>                  
                    o_address_next <= "0000000000000001";
                    o_en_next <= '1';
                    col_count:= to_integer(unsigned(i_data));
                    state_return_next <= ROW;
                    next_state <= WAIT_VALUE;                                  
                when ROW =>
                    o_address_next <= "0000000000000010";
                    o_en_next <= '1';
                    max_address_next <= (col_count * to_integer(unsigned(i_data))) + 1; 
                    count_next <= 3;                    
                    state_return_next <= MIN_MAX;
                    next_state <= WAIT_VALUE;    
                when MIN_MAX =>
                    if (to_integer(unsigned(i_data)) < min) then
                        min_next <= to_integer(unsigned(i_data));
                    end if;
                    if (to_integer(unsigned(i_data)) > max) then
                        max_next <=  to_integer(unsigned(i_data));
                    end if;
                    if (count = max_address+1) then
                        count_next <= 2; 
                        next_state <= ARG;
                    else
                        o_address_next <= std_logic_vector(to_unsigned(count, 16));
                        o_en_next <= '1';
                        count_next <= count + 1;
                        state_return_next <= MIN_MAX;
                        next_state <= WAIT_VALUE;
                    end if;                    
                when ARG =>
                    delta_value_next <= std_logic_vector(to_unsigned(max-min+1, 9));                    
                    next_state <= LOG;
                when LOG =>
                    i := 8;
                    while (i>0 and delta_value(i) = '0') loop
                        i := i-1;
                    end loop;
                    o_address_next <= "0000000000000010";
                    o_en_next <= '1';
                    shift_level_next <= 8-i;
                    count_next <= 1;
                    state_return_next <= UPDATE_READ;                                        
                    next_state <= WAIT_VALUE;               
                when UPDATE_READ =>
                    temp_pixel := std_logic_vector(unsigned(i_data) - to_unsigned(min, 8));
                    temp_pixel_shifted := std_logic_vector(resize(unsigned(temp_pixel), 16));
                    temp_pixel_shifted := std_logic_vector(shift_left(unsigned(temp_pixel_shifted), shift_level));
                    if (to_integer(unsigned(temp_pixel_shifted)) > 255) then
                        o_data_next <= (others => '1');
                    else
                        o_data_next <= std_logic_vector(resize(unsigned(temp_pixel_shifted), 8));
                    end if;
                    o_address_next <= std_logic_vector (to_unsigned(max_address+count, 16));
                    o_en_next <= '1';
                    o_we_next <= '1';
                    next_state <= UPDATE_WRITE;
                when UPDATE_WRITE =>
                    if(count = max_address - 1) then
                        o_done_next <= '1';
                        count_next <= 2;
                        min_next <= 255;
                        max_next <= 0;
                        max_address_next <= 2;
                        shift_level_next <= 0;
                        delta_value_next <= (others => '0');
                        state_return_next <= IDLE;
                        next_state <= DONE;
                    else
                        o_en_next <= '1';
                        o_address_next <= std_logic_vector (to_unsigned(2 + count, 16));
                        count_next <= count + 1;
                        state_return_next <= UPDATE_READ;
                        next_state <= WAIT_VALUE;
                    end if;
                when DONE =>
                    if (i_start = '0') then                                 
                        next_state <= IDLE;
                    else
                        next_state <= DONE;
                    end if;
            end case;
        end process;
               
end Behavioral;