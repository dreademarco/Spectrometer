library IEEE;
use IEEE.STD_LOGIC_1164.ALL;


entity constant_generator is
    Port ( clk : in STD_LOGIC;
           ce  : in STD_LOGIC;
           data_out : out STD_LOGIC_VECTOR (64 - 1 downto 0));
end constant_generator;

architecture Behavioral of constant_generator is

begin

registers : process(clk)
begin
    if (rising_edge(clk)) then
        if ce='1' then
            data_out <= (OTHERS => '1');
        else 
            data_out <= (OTHERS => '0');
        end if;
    end if;
end process;

end Behavioral;
