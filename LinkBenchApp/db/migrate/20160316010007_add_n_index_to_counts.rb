class AddNIndexToCounts < ActiveRecord::Migration
  def change
	add_index :counts, [:id1, :link_type], unique: true
  end
end
