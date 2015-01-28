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
-- Revision 1.00 - Version uno
-- Additional Comments:
--  <3 u andrea :P
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

type state_t is (idle, header, heap_id, pay_len, payload, eof);
signal current_state : state_t;

constant magic_number   : STD_LOGIC_VECTOR(8 - 1 downto 0)  := x"53";   -- ASCII character 'S'
constant version_number : STD_LOGIC_VECTOR(8 - 1 downto 0)  := x"04";   -- version 4.0
constant item_pointer_w : STD_LOGIC_VECTOR(8 - 1 downto 0)  := x"FF";   -- item pointer width
constant heap_addr_w    : STD_LOGIC_VECTOR(8 - 1 downto 0)  := x"FF";   -- head address width
constant reserved       : STD_LOGIC_VECTOR(16 - 1 downto 0) := x"0000"; -- reserved
constant nof_items      : STD_LOGIC_VECTOR(16 - 1 downto 0) := x"FFFF"; -- number of items

constant packet_header  : STD_LOGIC_VECTOR(64 - 1 downto 0) := magic_number & version_number & item_pointer_w & heap_addr_w & reserved & nof_items;

signal heap_number      : STD_LOGIC_VECTOR(64 - 1 downto 0) := x"0000000000000000";
constant payload_size   : integer := 8200;

signal eof_s, rst_s, valid_s : std_logic;
signal data_d, data_s : std_logic_vector (64 - 1 downto 0);
signal payload_s : std_logic_vector (64 - 1 downto 0);

signal state_number : std_logic_vector (8 - 1 downto 0);
signal state_reset  : std_logic := '1';

begin

    -- data generator according to switch
    dynamic : if count_sw = '1' generate
        generator : entity work.running_generator
        Port Map ( clk => clk,
                   ce  => not(state_reset),
                   data_out => data_d);   
    end generate;
    
    static : if count_sw = '0' generate             
        generator : entity work.constant_generator
        Port Map ( clk => clk,
                   ce  => not(state_reset),
                   data_out => data_s);   
    end generate;

    -- registers
    register_signals : process (clk) is
    begin
        if (rising_edge(clk)) then
            if (ce = '1') then
                tx_eof   <= eof_s;
                if ( count_sw = '0' ) then
                    tx_data  <= data_s;
                else
                    tx_data  <= data_d;
                end if;
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

    -- state number counter
    state_counter : entity work.counter 
    Generic Map ( count_size => 8 )
    Port Map (
        clk      => clk,
        ce       => ce,
        reset    => state_reset,
        data_out => state_number);

    -- control state machine
	change_state : process(clk, current_state)
	begin
		if (rising_edge(clk)) then
		    if (ce = '1') then
		        -- since each state is only 1 clock cycle long
		        -- we can change the state with just a flip-flop
		        if (current_state = idle) then
		            current_state <= header;
		        elsif (current_state = header) then
		            current_state <= heap_id;
		        elsif (current_state = heap_id) then
                    current_state <= pay_len;
                elsif (current_state = pay_len) then
                    current_state <= payload;
                elsif (current_state = payload and state_number = x"FF") then
                    current_state <= eof;
                elsif (current_state = eof) then
                    current_state <= idle;
		        end if;
		    else
		        current_state <= idle;
		    end if;
		end if;
	end process;

	output : process(current_state, heap_number, data_s)
	begin
		case current_state is
			when idle    =>
			    -- no data what so ever
				data_s      <= (OTHERS => '0');
				-- no end of frame
				eof_s       <= '0';
				-- no valid data
				valid_s     <= '0';
				-- reset states
				state_reset <= '1';
			when header  =>
			    -- send out spead header
				data_s      <= packet_header;
				-- no end of frame
				eof_s       <= '0';
				-- valid data
                valid_s     <= '1';
				-- no need to count for now
				state_reset <= '1';
				
				-- increment the heap number to indicate a new part of the heap
                heap_number <= heap_number + 1;
			when heap_id =>		
			    -- output the heap number	    
			    data_s      <= heap_number;
			    -- no end of frame
			    eof_s       <= '0';
			    -- valid data
                valid_s     <= '1';
			    -- no need to count for now
			    state_reset <= '1';
		    when pay_len  =>
		        -- output the size of the payload
                data_s   <= x"0000000000000101"; -- make dynamic at some point in time 
                -- no end of frame
                eof_s    <= '0';
                -- valid data
                valid_s     <= '1';
                -- remove reset from state couter
                state_reset <= '0';
			when payload =>
			    -- output actual data
				data_s   <= data_s;
				-- no end of frame
				eof_s    <= '0';
				-- valid data
                valid_s     <= '1';
				-- remove reset from state couter
                state_reset <= '0';
		    when eof =>
		        -- no data to output
		        data_s   <= (OTHERS => '0');
		        -- end of frame
		        eof_s    <= '1';
		        -- valid data
                valid_s     <= '1';
                -- no need to count for now
                state_reset <= '1';
		end case;
	end process;


end Behavioral;
