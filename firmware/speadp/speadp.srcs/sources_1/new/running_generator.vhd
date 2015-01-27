library IEEE;
use IEEE.STD_LOGIC_1164.ALL;


entity running_generator is
    Port ( clk : in STD_LOGIC;
           ce  : in STD_LOGIC;
           data_out : out STD_LOGIC_VECTOR (64 - 1 downto 0));
end running_generator;

architecture Behavioral of running_generator is

signal real_data_1, comp_data_1 : STD_LOGIC_VECTOR (8 - 1 downto 0);
signal real_data_2, comp_data_2 : STD_LOGIC_VECTOR (8 - 1 downto 0);
signal real_data_3, comp_data_3 : STD_LOGIC_VECTOR (8 - 1 downto 0);
signal real_data_4, comp_data_4 : STD_LOGIC_VECTOR (8 - 1 downto 0);

begin
        
    timesample_1 : entity work.heap_generator 
    Generic Map ( shift => 0 )
    Port Map ( clk           => clk,
               ce            => ce,
               data_out_real => real_data_1,
               data_out_comp => comp_data_1);
           
    timesample_2 : entity work.heap_generator 
    Generic Map ( shift => 1 )
    Port Map ( clk           => clk,
               ce            => ce,
               data_out_real => real_data_2,
               data_out_comp => comp_data_2);
                      
    timesample_3 : entity work.heap_generator 
    Generic Map ( shift => 2 )
    Port Map ( clk           => clk,
               ce            => ce,
               data_out_real => real_data_3,
               data_out_comp => comp_data_3);
                                 
    timesample_4 : entity work.heap_generator 
    Generic Map ( shift => 3 )
    Port Map ( clk           => clk,
               ce            => ce,
               data_out_real => real_data_4,
               data_out_comp => comp_data_4);
    
    registers : process(ce, real_data_1, comp_data_1, real_data_2, comp_data_2, real_data_3, comp_data_3, real_data_4, comp_data_4)
    begin
        if ce = '1' then
            data_out <= real_data_1 & comp_data_1 & real_data_2 & comp_data_2 & real_data_3 & comp_data_3 & real_data_4 & comp_data_4;
        else 
            data_out <= (OTHERS => '0');
        end if;
    end process;

end Behavioral;