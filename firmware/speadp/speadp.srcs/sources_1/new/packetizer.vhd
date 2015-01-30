----------------------------------------------------------------------------------
-- Company: Institute of Space Sciences and Astronomy (ISSA)
-- Physicist: Norbert Bonnici
-- 
-- Create Date: 01/07/2015 05:58:28 PM
-- Design Name: SPEADp
-- Module Name: SPEAD packetizer - Behavioral
-- Project Name: SPEAD generic packetizer
-- Target Devices: Behavioral
-- Tool Versions: 2014.4
-- Description: a simple data generator for 10G Ethernet designed for the ROACH 1 board
-- 
-- Dependencies: 
-- 
-- Revision:
-- Revision 1.01 - Version uno punto zero uno
-- Additional Comments:
--  
----------------------------------------------------------------------------------


library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.STD_LOGIC_ARITH.ALL;
use IEEE.STD_LOGIC_UNSIGNED.ALL;


entity packetizer is
    Generic ( count_sw : in STD_LOGIC := '1');
    Port ( clk : in STD_LOGIC;
           ce : in STD_LOGIC;
           reset : out STD_LOGIC;
           tx_data : out STD_LOGIC_VECTOR (64 - 1 downto 0);
           tx_valid : out STD_LOGIC;
           tx_eof : out STD_LOGIC);
end packetizer;

architecture Behavioral of packetizer is

-- changes for v1.01
-- added padding and removed payload length
type state_t is (idle, header, heap_id, payload, padding, eof); 
signal current_state : state_t;

constant frame_size     : integer := 256; -- work counter sizes
constant payload_size   : integer := 128; -- using constants

constant magic_number   : STD_LOGIC_VECTOR(8 - 1 downto 0)  := x"53";   -- ASCII character 'S'
constant version_number : STD_LOGIC_VECTOR(8 - 1 downto 0)  := x"04";   -- version 4.0
constant item_pointer_w : STD_LOGIC_VECTOR(8 - 1 downto 0)  := x"FF";   -- item pointer width
constant heap_addr_w    : STD_LOGIC_VECTOR(8 - 1 downto 0)  := x"FF";   -- heap address width
constant reserved       : STD_LOGIC_VECTOR(16 - 1 downto 0) := x"0000"; -- reserved
constant nof_items      : STD_LOGIC_VECTOR(16 - 1 downto 0) := x"0010"; -- number of 16 SPEAD items (16*8 samples per packet)

constant packet_header  : STD_LOGIC_VECTOR(64 - 1 downto 0) := magic_number & version_number & item_pointer_w & heap_addr_w & reserved & nof_items;

signal heap_number      : STD_LOGIC_VECTOR(64 - 1 downto 0) := (OTHERS => '0');

signal eof_s   : std_logic := '0';
signal rst_s   : std_logic := '0';
signal valid_s : std_logic := '0';
signal tx_data_s : std_logic_vector (64 - 1 downto 0) := (OTHERS => '0');
signal payload_s : std_logic_vector (64 - 1 downto 0) := (OTHERS => '0');

signal data_dyn : std_logic_vector (64 - 1 downto 0) := (OTHERS => '0');
signal data_stc : std_logic_vector (64 - 1 downto 0) := (OTHERS => '0');

signal state_number : std_logic_vector (4 - 1 downto 0) := (OTHERS => '0');
signal state_reset  : std_logic := '1';

signal idle_number : std_logic_vector (8 - 1 downto 0) := (OTHERS => '0');
signal idle_reset  : std_logic := '1';

signal padding_number : std_logic_vector (4 - 1 downto 0) := (OTHERS => '0');
signal padding_reset  : std_logic := '1';

begin

    -- data generator according to switch
    dynamic : if count_sw = '1' generate
        generator : entity work.running_generator
        Port Map ( clk => clk,
                   ce  => not(state_reset),
                   data_out => data_dyn);   
    end generate;
    
    static : if count_sw = '0' generate             
        generator : entity work.constant_generator
        Port Map ( clk => clk,
                   ce  => not(state_reset),
                   data_out => data_stc);   
    end generate;

    -- registers
    register_signals : process (clk) is
    begin
        if (rising_edge(clk)) then
            if (ce = '1') then
                tx_eof   <= eof_s;
                tx_data  <= tx_data_s;
                tx_valid <= valid_s;
                reset    <= '0';
            else 
                tx_eof   <= '0';
                tx_data  <= (OTHERS => '0');
                tx_valid <= '0';
                reset    <= '0'; -- never reset core!!!! 
            end if;
        end if;
    end process;

    -- idle delay counter
    delay_counter : entity work.counter 
    Generic Map ( count_size => 8 )
    Port Map (
        clk      => clk,
        reset    => idle_reset,
        data_out => idle_number);

    -- state number counter
    state_counter : entity work.counter 
    Generic Map ( count_size => 4 )
    Port Map (
        clk      => clk,
        reset    => state_reset,
        data_out => state_number);
        
    -- padding number counter
    padding_counter : entity work.counter 
    Generic Map ( count_size => 4 )
    Port Map (
        clk      => clk,
        reset    => padding_reset,
        data_out => padding_number);

    -- control state machine
	change_state : process(clk, current_state)
	begin
		if (rising_edge(clk)) then
		    if (ce = '1') then
		        -- since each state is only 1 clock cycle long
		        -- we can change the state with just a flip-flop
		        
		        -- initial state is always idle and goes to header (256 clock cycles - no data)
		        if (current_state = idle and idle_number = x"FF") then
		            current_state <= header;
		        
		        -- send heap id after header (1 clock cycle - 8 bytes)
		        elsif (current_state = header) then
		            current_state <= heap_id;
		            
		        -- send payload after heap_id (1 clock cycle - 8 bytes)
		        elsif (current_state = heap_id) then
                    current_state <= payload;
                    
--                elsif (current_state = pay_len) then
--                    current_state <= payload;

                -- send padding after payload (16 clock cycles - 128 bytes)
                elsif (current_state = payload and state_number = x"F") then
                    current_state <= padding;
                    
                -- send eof after padding (12 clock cycles - 96 bytes of zeros)
                elsif (current_state = padding and padding_number = x"A") then
                    current_state <= eof;
                    
                -- go back to idle mofo (1 clock cycle)
                elsif (current_state = eof) then
                    current_state <= idle;
		        end if;
		    else
		        current_state <= idle;
		    end if;
		end if;
	end process;

	output : process(current_state, heap_number, tx_data_s)
	begin
		case current_state is
			when idle    =>
			    -- no data what so ever
				tx_data_s   <= (OTHERS => '0');
				-- no end of frame
				eof_s       <= '0';
				-- no valid data
				valid_s     <= '0';
				-- reset states
				idle_reset    <= '0';
				state_reset   <= '1';
				padding_reset <= '1';

			when header  =>
			    -- send out spead header
				tx_data_s   <= packet_header;
				-- no end of frame
				eof_s       <= '0';
				-- valid data
                valid_s     <= '1';
				-- no need to count for now
				idle_reset    <= '1';
				state_reset   <= '1';
				padding_reset <= '1';
				-- increment the heap number to indicate a new part of the heap
                heap_number <= heap_number + 1;
                
			when heap_id =>		
			    -- output the heap number	    
			    tx_data_s   <= heap_number;
			    -- no end of frame
			    eof_s       <= '0';
			    -- valid data
                valid_s     <= '1';
			    -- remove reset from state couter
			    state_reset <= '0';
			    -- no need to count for now
			    idle_reset  <= '1';
			    padding_reset <= '1';
			    
--		    when pay_len  =>
--		        -- output the size of the payload
--                tx_data_s   <= x"0000000000000101"; -- make dynamic at some point in time 
--                -- no end of frame
--                eof_s       <= '0';
--                -- valid data
--                valid_s     <= '1';
--                -- remove reset from state couter
--                state_reset <= '0';
--                idle_reset  <= '1';

			when payload =>
			    -- output actual data
			    if ( count_sw = '0' ) then
                    tx_data_s  <= data_stc;
                else
                    tx_data_s  <= data_dyn;
                end if;
				-- no end of frame
				eof_s    <= '0';
				-- valid data
                valid_s     <= '1';
                -- no need to count now
                idle_reset  <= '1';
				-- remove reset from state couter
                state_reset <= '0';
                -- remove reset from padding couter if last state
                if (state_number = x"E") then
                    padding_reset <= '0';
                else
                    padding_reset <= '1';
                end if;
                
            when padding =>
                -- output actual data
                tx_data_s   <= (OTHERS => '0');
                -- no end of frame
                eof_s       <= '0';
                -- valid data
                valid_s     <= '1';
                -- no need to count for now
                state_reset <= '1';
                idle_reset  <= '1';
                -- remove reset from padding couter
                padding_reset <= '0';
                
		    when eof =>
		        -- no data to output
		        tx_data_s   <= (OTHERS => '0');
		        -- end of frame
		        eof_s       <= '1';
		        -- valid data
                valid_s     <= '1';
                -- no need to count for now
                state_reset <= '1';
                padding_reset <= '1';
                -- prepeare idle count
                idle_reset  <= '0';                
		end case;
	end process;


end Behavioral;
