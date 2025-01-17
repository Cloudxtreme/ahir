-- author: Madhav P. Desai
library ieee;
use ieee.std_logic_1164.all;

package Utility_Package is

  -----------------------------------------------------------------------------
  -- constants
  -----------------------------------------------------------------------------
  constant c_word_length : integer := 32;
  constant c_vhpi_max_string_length : integer := 1024;
  
  -----------------------------------------------------------------------------
  -- types
  -----------------------------------------------------------------------------
  subtype VhpiString is string(1 to c_vhpi_max_string_length);

  -----------------------------------------------------------------------------
  -- utility functions
  -----------------------------------------------------------------------------
  function Minimum(x,y: integer) return integer; -- returns minimum
  function Pack_String_To_Vhpi_String(x: string) return VhpiString; -- converts x to null terminated string
  function Pack_SLV_To_Vhpi_String(x: std_logic_vector) return VhpiString; -- converts slv x to null terminated string
  function Unpack_String(x: VhpiString; lgth: integer) return std_logic_vector; -- convert null term string to slv
  function To_Std_Logic(x: VhpiString) return std_logic; -- string to sl
  function To_String(x: std_logic) return VhpiString; -- string to sl
  function Convert_To_String(val : natural) return STRING; -- convert val to string.

end package Utility_Package;

package body Utility_Package is

  -----------------------------------------------------------------------------
  -- utility functions
  -----------------------------------------------------------------------------
  function Minimum(x,y: integer) return integer is
  begin
    if( x < y) then return x; else return y; end if;
  end Minimum;

  function Ceiling(x,y: integer) return integer is
    variable ret_var : integer;
  begin
    assert x /= 0 report "divide by zero in ceiling function" severity failure;
    ret_var := x/y;
    if(ret_var*y < x) then ret_var := ret_var + 1; end if;
    return(ret_var);
  end Ceiling;

  function Pack_String_To_Vhpi_String(x:  string) return VhpiString is
     alias lx: string(1 to x'length) is x;
     variable strlen: integer;
     variable ret_var : VhpiString;
  begin
	strlen := Minimum(c_vhpi_max_string_length-1,x'length);
	for I in 1 to strlen loop
		ret_var(I) := lx(I);
	end loop;
	ret_var(strlen+1) := nul;
	return(ret_var); 
  end Pack_String_To_Vhpi_String;
  
  function Pack_SLV_To_Vhpi_String(x: std_logic_vector) return VhpiString is
    alias lx : std_logic_vector(1 to x'length) is x;
    variable strlen: integer;
    variable ret_var : VhpiString;
  begin
    strlen := Minimum(c_vhpi_max_string_length-1,x'length);
    for I in 1 to strlen loop
      if(lx(I) = '1') then 
        ret_var(I) := '1';
      else
        ret_var(I) := '0';
      end if;
    end loop;
    ret_var(strlen+1) := nul;
    return(ret_var); 
  end Pack_SLV_To_Vhpi_String;
  
  function Unpack_String(x: VhpiString; lgth: integer) return std_logic_vector is
    variable ret_var : std_logic_vector(1 to lgth);
    variable strlen: integer;
  begin
    strlen := Minimum(c_vhpi_max_string_length-1,lgth);
    for I in 1 to strlen loop
      if(x(I) = '1') then 
        ret_var(I) := '1';
      else
        ret_var(I) := '0';
      end if;
    end loop;
    return(ret_var);     
  end Unpack_String;

  function To_Std_Logic(x: VhpiString) return std_logic is
    variable s: std_logic_vector(0 downto 0);
  begin
    s := Unpack_String(x,1);
    return(s(0));
  end To_Std_Logic;

  function To_String(x: std_logic) return VhpiString is
    variable s: std_logic_vector(0 downto 0);
  begin
   s(0) := x;
   return(Pack_SLV_To_Vhpi_String(s));
  end To_String;

  -- Thanks to: D. Calvet calvet@hep.saclay.cea.fr
  function Convert_To_String(val : NATURAL) return STRING is
	variable result : STRING(10 downto 1) := (others => '0'); -- smallest natural, longest string
	variable pos    : NATURAL := 1;
	variable tmp, digit  : NATURAL;
  begin
	tmp := val;
	loop
		digit := abs(tmp MOD 10);
	    	tmp := tmp / 10;
	    	result(pos) := character'val(character'pos('0') + digit);
	    	pos := pos + 1;
	    	exit when tmp = 0;
	end loop;
	return result((pos-1) downto 1);
  end Convert_To_String;

end Utility_Package;

