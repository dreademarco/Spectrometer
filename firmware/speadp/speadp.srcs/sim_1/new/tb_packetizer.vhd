----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 01/28/2015 03:13:17 PM
-- Design Name: 
-- Module Name: tb_packetizer - Behavioral
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

-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx leaf cells in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity tb_packetizer is
--  Port ( );
end tb_packetizer;

architecture Behavioral of tb_packetizer is

-- Component Declaration for the Unit Under Test (UUT)
 
    COMPONENT packetizer
    PORT(
         clk : IN  std_logic;
         ce : IN  std_logic;
         reset : OUT  std_logic;
         tx_data : OUT  std_logic_vector(63 downto 0);
         tx_valid : OUT  std_logic;
         tx_eof : OUT  std_logic
        );
    END COMPONENT;
    

   --Inputs
   signal clk : std_logic := '0';
   signal ce : std_logic := '0';

 	--Outputs
   signal reset : std_logic;
   signal tx_data : std_logic_vector(63 downto 0);
   signal tx_valid : std_logic;
   signal tx_eof : std_logic;

   -- Clock period definitions
   constant clk_period : time := 10 ns;
 
BEGIN
 
	-- Instantiate the Unit Under Test (UUT)
   uut: packetizer PORT MAP (
          clk => clk,
          ce => ce,
          reset => reset,
          tx_data => tx_data,
          tx_valid => tx_valid,
          tx_eof => tx_eof
        );

   -- Clock process definitions
   clk_process :process
   begin
		clk <= '0';
		wait for clk_period/2;
		clk <= '1';
		wait for clk_period/2;
   end process;
 

   -- Stimulus process
   stim_proc: process
   begin		
      -- hold reset state for 100 ns.
      wait for 100 ns;	

      wait for clk_period*10;
		
		ce <= '1';

      -- insert stimulus here 

      wait;
   end process;

end Behavioral;
