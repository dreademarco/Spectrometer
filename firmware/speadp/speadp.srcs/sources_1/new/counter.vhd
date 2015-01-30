library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.STD_LOGIC_ARITH.ALL;
use IEEE.STD_LOGIC_UNSIGNED.ALL;


entity counter is
    Generic ( count_size   : in integer := 8;
              increment_by : in integer := 1 );
    Port ( clk      : in STD_LOGIC;
           reset    : in STD_LOGIC;
           data_out : out STD_LOGIC_VECTOR (count_size - 1 downto 0));
end counter;

architecture Behavioral of counter is

signal count : STD_LOGIC_VECTOR (count_size - 1 downto 0);
constant max_count : STD_LOGIC_VECTOR (count_size - 1 downto 0) := (OTHERS => '1');

begin

    registers : process(clk)
    begin
        if (rising_edge(clk)) then
            data_out <= count;
        end if;
    end process;
    
    thingamijiggy : process(clk, reset)
    begin
        if reset = '1' then
            count <= (OTHERS => '0');
        elsif (rising_edge(clk)) then
            if count = max_count then 
                count <= (OTHERS => '0');
            else
                count <= count + increment_by;
            end if;
        end if;
    end process;

end Behavioral;
